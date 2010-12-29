/**
 * @file
 * @date Dec 29, 2010
 */

#include "anyterm/activity.hpp"

#include <boost/iostreams/char_traits.hpp>
#include <boost/bind.hpp>

#include <iostream>

namespace anyterm {
  ::asio::io_service activity_base::io_service_;

  activity_base::activity_base(on_output_slot_t on_output_in,
                               on_error_slot_t on_error_in,
                               ::boost::shared_ptr< ::asiop::stream_descriptor > stream_inout) :
    __stream_ptr(stream_inout), __terminating(false) {
    ::std::clog << "activity_base()" << ::std::endl;
    __on_output.connect(on_output_in);
    __on_error.connect(on_error_in);
  }

  activity_base::~activity_base() {
    ::std::clog << "~activity_base()" << ::std::endl;
    ::boost::lock_guard< ::boost::mutex > guard(__mutex);

    if (__stream_ptr->is_open()) {
      __stream_ptr->close();
    }
    __on_output.disconnect_all_slots();
    __on_error.disconnect_all_slots();
  }

  void activity_base::send(::std::string const& string_in) {
    ::asio::write(*__stream_ptr, ::asio::buffer(string_in));
  }

  void activity_base::process_output() {
    if (!__terminating && __stream_ptr->is_open()) {
      ::boost::lock_guard< ::boost::mutex > guard(__mutex);

      char buffer[BUFSIZ];
      memset(buffer, 0, sizeof(buffer));

      try {
        if (__stream_ptr->read_some(::asio::buffer(buffer, sizeof(buffer))) > 0) {
          __on_output(::std::string(buffer));
        }
      } catch (::boost::system::system_error const& e) {
        __on_error("Subprocess terminated.");
        __terminating = true;
      }
    }
  }

} // namespace anyterm
