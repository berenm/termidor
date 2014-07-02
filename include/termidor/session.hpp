/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#ifndef __TERMIDOR_SESSION_HPP__
#define __TERMIDOR_SESSION_HPP__

#include <cstdint>
#include <string>
#include <chrono>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "termidor/screen.hpp"
#include "termidor/terminal.hpp"

namespace termidor {
  typedef std::chrono::system_clock clock;

  struct session : boost::noncopyable {
    public:
      session(std::string const& username, std::uint16_t const row_count, std::uint16_t const column_count, std::uint16_t const scrollback_count=0, std::chrono::seconds const timeout=std::chrono::seconds(60));
      ~session();

      void resize(std::uint16_t const row_count, std::uint16_t const column_count);

      void        write(std::string const& data);
      std::string read();

      void touch();
      bool timed_out();

    private:
      std::string const          username;
      std::chrono::seconds const timeout;
      clock::time_point          last_access;
      termidor::terminal          terminal;

  };
  typedef boost::shared_ptr< session > session_ptr;

} // namespace termidor

#endif // ifndef __TERMIDOR_SESSION_HPP__
