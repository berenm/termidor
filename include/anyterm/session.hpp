/**
 * @file
 * @date Dec 29, 2010
 */

#ifndef ANYTERM_SESSION_HPP_
#define ANYTERM_SESSION_HPP_

#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "anyterm/screen.hpp"
#include "anyterm/terminal.hpp"
#include "anyterm/activity.hpp"
//#include "Iconver.hh"

#include <iostream>
#include <boost/algorithm/string.hpp>

namespace anyterm {

  struct debug_printer: ::std::unary_function< void, char > {
      void operator()(char const char_in) {
        if (::boost::is_print()(char_in)) {
          ::std::clog << char_in;
        } else {
          ::std::clog << "\\x" << ::std::hex << ::std::setfill('0') << ::std::setw(2)
              << static_cast< ::std::uint32_t > (char_in) << ::std::dec << ::std::setw(0);
        }
      }
  };

  class session: ::boost::noncopyable {
    public:
      ::std::uint8_t const __row_count;
      ::std::uint8_t const __column_count;
      ::std::uint16_t const __scrollback_count;
      ::std::uint32_t const __timeout;

    public:
      volatile time_t last_access;
      terminal __terminal;

      session(::std::uint8_t const row_count_in = 25,
              ::std::uint8_t const column_count_in = 80,
              ::std::uint16_t const scrollback_count_in = 0,
              ::std::uint32_t const timeout_in = 60);
      ~session();

      void touch();
      void report_any_backend_error();
      void send(::std::string k);
      ::std::string receive();

      bool timed_out();
  };

} // namespace anyterm

#endif /* ANYTERM_SESSION_HPP_ */
