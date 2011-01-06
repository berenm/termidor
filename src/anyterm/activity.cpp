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

  activity_base::activity_base(on_output_slot_t on_output_in, on_error_slot_t on_error_in) :
    __started(false) {
    ::std::clog << "activity_base()" << ::std::endl;
    __on_output.connect(on_output_in);
    __on_error.connect(on_error_in);
  }

  activity_base::~activity_base() {
    ::boost::lock_guard< ::boost::mutex > guard(__mutex);

    ::std::clog << "~activity_base()" << ::std::endl;
    stop();

    __on_output.disconnect_all_slots();
    __on_error.disconnect_all_slots();
  }

  void activity_base::start() {
    //    ::boost::lock_guard< ::boost::mutex > guard(__mutex);

    __started = true;
  }

  void activity_base::stop() {
    //    ::boost::lock_guard< ::boost::mutex > guard(__mutex);

    __started = false;
  }

  bool activity_base::is_started() {
    return __started;
  }

  bool activity_base::is_good() {
    return __started && __stream_ptr && __stream_ptr->is_open();
  }

  bool activity_base::has_output() {
    if (is_good()) {
      ::asiop::descriptor_base::bytes_readable bytes_readable;
      __stream_ptr->io_control(bytes_readable);
      return bytes_readable.get() > 0;
    } else {
      return false;
    }
  }

  void activity_base::send(::std::string const& string_in) {
    if (is_good()) {
      //      ::boost::lock_guard< ::boost::mutex > guard(__mutex);
      ::asio::write(*__stream_ptr, ::asio::buffer(string_in));
    }
  }

  void activity_base::process_output() {
    ::boost::lock_guard< ::boost::mutex > guard(__mutex);

    char buffer[BUFSIZ];
    memset(buffer, 0, sizeof(buffer));
    try {
      //      ::boost::lock_guard< ::boost::mutex > guard(__mutex);

      if (is_good()) {
        if (__stream_ptr->read_some(::asio::buffer(buffer, sizeof(buffer))) > 0) {
          __on_output(::std::string(buffer));
        }
      }
    } catch (::boost::system::system_error const& e) {
      __on_error("Subprocess terminated.");
      stop();
    }
  }

} // namespace anyterm
