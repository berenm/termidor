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

#include "config.hh"
#include "anyterm/html.hpp"
#include "anyterm/editscript.hpp"
#include "Iconver.hh"
#include "unicode.hh"

namespace anyterm {

  session::session(::std::uint8_t const row_count_in,
                   ::std::uint8_t const column_count_in,
                   ::std::uint16_t const scrollback_count_in,
                   ::std::uint32_t const timeout_in,
                   activity_factory_base const& activity_factory_in,
                   ::std::string const& charset_in,
                   bool const diff_in) :
    row_count(row_count_in), column_count(column_count_in),
        scrollback_count(scrollback_count_in),
        timeout(timeout_in),
        charset(charset_in),
        diff(diff_in)
        //        , utf8_to_charset("UTF-8", charset)
        //        , charset_to_ucs4(charset, UCS4_NATIVE)
        //        , ucs4_to_utf8(UCS4_NATIVE, "UTF-8")
        , screen_(row_count, column_count, scrollback_count), error(false), __terminal(row_count,
                                                                                       column_count,
                                                                                       screen_),
        activity(activity_factory_in(::boost::bind(&session::process_output, this, _1),
                                     ::boost::bind(&session::process_error, this, _1))) {
    ::std::clog << "session()" << ::std::endl;
    dirty = true;
    touch();
  }

  session::~session() {
    ::std::clog << "~session()" << ::std::endl;
  }

  // Timeout unused sessions:
  void session::touch(void) {
    last_access = time(NULL);
  }

  // Check if session has a backend error to report
  void session::report_any_backend_error(void) {
    if (error) {
      error = false;
      throw error_message;
    }
  }

  // Handle Apache requests:
  ::std::string int_to_string(int i) {
    char b[32];
    snprintf(b, sizeof(b), "%d", i);
    return b;
  }

  void session::send(::std::string k) {
    ::std::clog << "Writing input:'" << k << "'" << ::std::endl;

    if (!k.empty()) {
      // FIXME:      ::std::string dk = utf8_to_charset(k);
      ::std::string dk = k;
      activity->send(dk);
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

  ::std::string session::rcv(void) {
    {
      ::boost::unique_lock< ::boost::mutex > lock(screen_mutex);

      if (!dirty && !error) {
        dirty_condition.timed_wait(lock, ::boost::posix_time::seconds(10));
      }
    }
    touch();

    report_any_backend_error();

    bool was_dirty;
    ucs4_string html_screen;
    {
      ::boost::lock_guard< ::boost::mutex > lock(screen_mutex);

      was_dirty = dirty;
      dirty = false;
      if (!was_dirty) {
        return "n";
      }

      html_screen = htmlify_screen(screen_);
    }

    ::std::string utf8_editscript;
    if (diff) {
      ucs4_string editscript = make_editscript(old_html_screen, html_screen);
      old_html_screen = html_screen;
      // FIXME:      utf8_editscript = ucs4_to_utf8(editscript);
      utf8_editscript.assign(editscript.begin(), editscript.end());
    } else {
      utf8_editscript = "R";
      // FIXME:      utf8_editscript.append(ucs4_to_utf8(html_screen));
      ::std::string app(html_screen.begin(), html_screen.end());
      utf8_editscript.append(app);
    }

    return utf8_editscript;
  }

  void session::process_output(::std::string const& string_in) {
    ::std::clog << "Got output: '" << string_in << "'" << ::std::endl;
    // FIXME:          ucs4_string ucs4_s = charset_to_ucs4(string_in);
    ucs4_string ucs4_s;
    ucs4_s.assign(string_in.begin(), string_in.end());
    {
      ::boost::lock_guard< ::boost::mutex > lock(screen_mutex);

      __terminal.write(ucs4_s);
      dirty = true;
    }

    dirty_condition.notify_all();
  }

  void session::process_error(::std::string const& string_in) {
    ::std::clog << "Got error: '" << string_in << "'" << ::std::endl;
    // We could have a lock here, but maybe we can trust that the assignment
    // to error is atomic.
    error_message = string_in;
    error = true;

    dirty_condition.notify_all();
  }

  bool session::timed_out(void) {
    return time(NULL) - last_access > timeout;
  }

} // namespace anyterm
