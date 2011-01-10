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

#include <iostream>

namespace anyterm {

  session::session(::std::string const& username_in,
                   ::std::uint16_t const row_count_in,
                   ::std::uint16_t const column_count_in,
                   ::std::uint16_t const scrollback_count_in,
                   ::std::uint32_t const timeout_in) :
    __username(username_in), __timeout(timeout_in), __terminal() {
    __terminal.set_size(row_count_in, column_count_in);
    __terminal.set_scrollback_size(scrollback_count_in);
    touch();
  }

  session::~session() {
  }

  void session::touch() {
    last_access = time(NULL);
  }

  ::std::string int_to_string(int i) {
    char b[32];
    snprintf(b, sizeof(b), "%d", i);
    return b;
  }

  void session::resize(::std::uint16_t const row_count_in, ::std::uint16_t const column_count_in) {
    __terminal.set_size(row_count_in, column_count_in);
  }

  void session::write(::std::string const& data_in) {
    if (!__terminal.is_alive()) {
      __terminal.login(__username);
    }

    if (!data_in.empty()) {
      __terminal.write(data_in);
    }

    touch();
  }

  ::std::string session::read() {
    if (!__terminal.is_alive()) {
      __terminal.login(__username);
    }

    if (__terminal.is_dirty()) {
      screen current_screen = __terminal.read();

      return htmlify_screen(current_screen);
    }

    return "";
  }

  bool session::timed_out() {
    return time(NULL) - last_access > __timeout;
  }

} // namespace anyterm
