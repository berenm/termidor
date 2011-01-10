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
//#include "Iconver.hh"

#include <iostream>
#include <boost/algorithm/string.hpp>

namespace anyterm {

  class session: ::boost::noncopyable {
    public:
      ::std::string const __username;
      ::std::uint32_t const __timeout;

    public:
      volatile time_t last_access;
      terminal __terminal;

      session(::std::string const& username_in,
              ::std::uint16_t const row_count_in,
              ::std::uint16_t const column_count_in,
              ::std::uint16_t const scrollback_count_in = 0,
              ::std::uint32_t const timeout_in = 60);
      ~session();

      void touch();
      void report_any_backend_error();

      void resize(::std::uint16_t const row_count_in, ::std::uint16_t const column_count_in);

      void write(::std::string const& data_in);
      ::std::string read();

      bool timed_out();
  };

} // namespace anyterm

#endif /* ANYTERM_SESSION_HPP_ */
