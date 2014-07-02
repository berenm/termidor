#include "termidor/session_manager.hpp"

#include <boost/uuid/uuid_io.hpp>

#include <gtk/gtk.h>

namespace termidor {
  namespace {

    static gboolean work_static(void* data) {
      static_cast< termidor::session_manager* >(data)->work();

      return true;
    }

  }

  session_manager::session_manager() :
    terminated(false),
    generate_random(std::mt19937(termidor::clock::now().time_since_epoch().count()) ()),
    generate_uuid(generate_random) {
    g_timeout_add(100, work_static, this);
    background_worker = boost::thread([] {
                                        gtk_main();
                                      });
  }

  session_manager::~session_manager() {
    sessions.clear();
    this->terminated = true;
    background_worker.timed_join(boost::posix_time::seconds(1));
  }

  std::string session_manager::new_session(std::string const& username, std::uint16_t const row_count, std::uint16_t const column_count) {
    boost::uuids::uuid const& session_uuid = this->generate_uuid();
    std::string const&        session_id   = boost::uuids::to_string(session_uuid);

    sessions[session_id].reset(new session(username, row_count, column_count));

    return session_id;
  }

  session_ptr session_manager::get_session(std::string const& session_id) {
    return sessions[session_id];
  }

  void session_manager::work() {
    if (this->terminated)
      gtk_main_quit();

    for (auto it = this->sessions.begin(), end = this->sessions.end(); it != end;) {
      if (it->second->timed_out())
        this->sessions.erase(it++);
      else
        ++it;
    }
  }

}
