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
#include <boost/thread.hpp>

namespace anyterm {

  static gboolean work_static(void* session_manager_in);

  static void start_gtk() {
    gtk_main();
  }

  struct session_manager {
      volatile bool __terminated;
      ::boost::thread __background_worker;

      session_manager() :
        __terminated(false) {
        g_timeout_add(100, work_static, this);
        __background_worker = ::boost::thread(start_gtk);
      }

      ~session_manager() {
        __sessions.clear();
        __terminated = true;
        __background_worker.timed_join(::boost::posix_time::seconds(1));
      }

      void new_session(::std::string const& session_id_in,
                       ::std::uint8_t const row_count_in,
                       ::std::uint8_t const column_count_in) {
        __sessions[session_id_in].reset(new session(row_count_in, column_count_in));
      }

      ::boost::shared_ptr< session > get_session(::std::string const& session_id_in) {
        return __sessions[session_id_in];
      }

      void work() {
        if (__terminated) {
          gtk_main_quit();
        }
      }

    private:
      typedef ::std::map< ::std::string, ::boost::shared_ptr< session > > session_map_t;
      typedef session_map_t::value_type session_map_pair_t;

      session_map_t __sessions;

      struct clear_timed_out: ::std::unary_function< session_map_pair_t, void > {
          clear_timed_out(session_map_t& sessions_in) :
            __sessions(sessions_in) {
          }

          void operator()(session_map_pair_t& pair_inout) {
            if (pair_inout.second && pair_inout.second->timed_out()) {
              pair_inout.second.reset();
            }

            if (!pair_inout.second) {
              session_map_t::iterator session_it = __sessions.find(pair_inout.first);
              if (session_it != __sessions.end()) {
                __sessions.erase(session_it);
              }
            }
          }

        private:
          session_map_t& __sessions;
      };
  };

  static gboolean work_static(void* session_manager_in) {
    reinterpret_cast< session_manager* > (session_manager_in)->work();
    return true;
  }

} // namespace anyterm

#endif /* ANYTERM_SESSION_MANAGER_HPP_ */
