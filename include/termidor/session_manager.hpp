/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#ifndef __TERMIDOR_SESSION_MANAGER_HPP__
#define __TERMIDOR_SESSION_MANAGER_HPP__

#include <cstdint>
#include <string>
#include <unordered_map>

#include <boost/thread.hpp>
#include <boost/uuid/random_generator.hpp>
#include <random>

#include "termidor/session.hpp"

namespace termidor {

  struct session_manager {
    public:
      session_manager();
      ~session_manager();
      std::string new_session(std::string const& username, std::uint16_t const row_count, std::uint16_t const column_count);
      session_ptr get_session(std::string const& session_id);
      void        work();

    private:
      typedef std::unordered_map< std::string, termidor::session_ptr > sessions_map;

      sessions_map  sessions;
      volatile bool terminated;
      boost::thread background_worker;

      std::mt19937                                         generate_random;
      boost::uuids::basic_random_generator< std::mt19937 > generate_uuid;
  };

} // namespace termidor

#endif // ifndef __TERMIDOR_SESSION_MANAGER_HPP__
