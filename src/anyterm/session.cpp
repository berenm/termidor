/**
 * @file
 * @date Dec 29, 2010
 */

#include "anyterm/session.hpp"

#include <sstream>
#include <iomanip>

#include <unistd.h>
#include <signal.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "anyterm/html.hpp"
#include "anyterm/editscript.hpp"

#include <iostream>

namespace anyterm {

  session::session(::std::uint8_t const row_count_in,
                   ::std::uint8_t const column_count_in,
                   ::std::uint16_t const scrollback_count_in,
                   ::std::uint32_t const timeout_in) :
    __row_count(row_count_in), __column_count(column_count_in), __scrollback_count(scrollback_count_in),
        __timeout(timeout_in), __terminal(row_count_in, column_count_in, scrollback_count_in) {
    ::std::clog << "session()" << ::std::endl;
    __terminal.fork_command("su username -");
    touch();
  }

  session::~session() {
    ::std::clog << "~session()" << ::std::endl;
  }

  void session::touch() {
    last_access = time(NULL);
  }

  ::std::string int_to_string(int i) {
    char b[32];
    snprintf(b, sizeof(b), "%d", i);
    return b;
  }

  void session::send(::std::string k) {
    if (!k.empty()) {
      __terminal.write(k);
    }

    touch();
  }

  ::std::string escape_html(::std::string s) {
    ::std::string t;
    for (::std::string::size_type i = 0; i < s.length(); i++) {
      char c = s[i];
      switch (c) {
        case '<':
          t += "&lt;";
          break;
        case '>':
          t += "&gt;";
          break;
        case '&':
          t += "&amp;";
          break;
        default:
          t += c;
          break;
      }
    }
    return t;
  }

  ::std::string session::receive() {
    screen current_screen = __terminal.read();

    ::std::string const html = htmlify_screen(current_screen);
    if (__last_screen_html.compare(html)) {
      __last_screen_html = html;

      return html;
    }

    return __last_screen_html;
  }

  bool session::timed_out() {
    return time(NULL) - last_access > __timeout;
  }

} // namespace anyterm
