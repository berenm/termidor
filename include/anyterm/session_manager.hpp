/**
 * @file
 * @date Dec 29, 2010
 */

#ifndef ANYTERM_SESSION_MANAGER_HPP_
#define ANYTERM_SESSION_MANAGER_HPP_

#include "anyterm/session.hpp"
#include "anyterm/activity/sub_process.hpp"

#include <map>

#include <boost/shared_ptr.hpp>

namespace anyterm {

  struct session_manager {
      volatile bool __terminated;
      ::boost::thread __background_worker;

      session_manager() :
        __terminated(false), __background_worker(&session_manager::background_work, this) {
      }

      ~session_manager() {
        __terminated = true;
        __background_worker.join();
      }

      void new_session(::std::string const& session_id_in) {
        __sessions[session_id_in].reset(new session(25, 80, 0, 60, sub_process_factory(""), "ascii", true));
      }

      ::boost::shared_ptr< session > get_session(::std::string const& session_id_in) {
        return __sessions[session_id_in];
      }

      void background_work() {
        while (!__terminated) {
          cleanup();
          process_outputs();
          ::boost::this_thread::sleep(::boost::posix_time::milliseconds(100));
        }
      }

      void cleanup() {
        session_map_t::iterator session_it;

        for (session_it = __sessions.begin(); session_it != __sessions.end(); ++session_it) {
          if (session_it->second && session_it->second->timed_out()) {
            session_it->second.reset();
          }
        }

        kill(getpid(), SIGCHLD);
      }

      void process_outputs() {
        session_map_t::iterator session_it;

        for (session_it = __sessions.begin(); session_it != __sessions.end(); ++session_it) {
          if (session_it->second) {
            session_it->second->activity->process_output();
          }
        }
      }

    private:
      typedef ::std::map< ::std::string, ::boost::shared_ptr< session > > session_map_t;
      session_map_t __sessions;
  };

} // namespace anyterm

#endif /* ANYTERM_SESSION_MANAGER_HPP_ */
