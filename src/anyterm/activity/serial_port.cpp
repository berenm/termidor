/**
 * @file
 * @date Dec 29, 2010
 */

#include "anyterm/activity/serial_port.hpp"
#include "anyterm/error.hpp"

#include <termios.h>
#include <unistd.h>

#include <iostream>

namespace anyterm {
  namespace activity {

  //    serial_port::serial_port(activity_base::on_output_slot_t on_output_in,
  //                             activity_base::on_error_slot_t on_error_in,
  //                             ::std::string const& filename_in,
  //                             ::std::uint32_t const baudrate_in) :
  //      activity_base(on_output_in, on_error_in), __filename(filename_in), __baudrate(baudrate_in) {
  //      ::std::clog << "serial_port()" << ::std::endl;
  //    }
  //
  //    serial_port::~serial_port() {
  //      ::std::clog << "~serial_port()" << ::std::endl;
  //      stop();
  //    }
  //
  //    void serial_port::start() {
  //      __stream_ptr.reset(new ::asiop::stream_descriptor(activity_base::io_service_,
  //                                                        ::bio::file_descriptor(__filename).handle()));
  //
  //      // Quote from the glibc docs:
  //      //   In the GNU library, the functions [cfsetispeed, cfsetospeed, etc] accept
  //      //   speeds measured in bits per second as input, and return speed values
  //      //   measured in bits per second. Other libraries require speeds to be indicated
  //      //   by special codes.
  //      // So this code will compile, but probably not work, on non-glibc systems.
  //
  //      struct termios settings;
  //      int rc = tcgetattr(__stream_ptr->native(), &settings);
  //      if (rc < 0) {
  //        throw error() << ::boost::errinfo_api_function("tcgetattr") << ::boost::errinfo_file_name(__filename)
  //            << ::boost::errinfo_errno(errno);
  //      }
  //
  //      // The following is equivalent to the non-portable cfmakeraw(&settings)
  //      settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  //      settings.c_oflag &= ~OPOST;
  //      settings.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  //      settings.c_cflag &= ~(CSIZE | PARENB);
  //      settings.c_cflag |= CS8;
  //
  //      rc = cfsetospeed(&settings, __baudrate);
  //      if (rc != 0) {
  //        throw error() << ::boost::errinfo_api_function("cfsetospeed")
  //            << ::boost::errinfo_file_name(__filename) << ::boost::errinfo_errno(errno);
  //      }
  //
  //      rc = cfsetispeed(&settings, __baudrate);
  //      if (rc != 0) {
  //        throw error() << ::boost::errinfo_api_function("cfsetispeed")
  //            << ::boost::errinfo_file_name(__filename) << ::boost::errinfo_errno(errno);
  //      }
  //
  //      rc = tcsetattr(__stream_ptr->native(), TCSANOW, &settings);
  //      if (rc < 0) {
  //        throw error() << ::boost::errinfo_api_function("tcsetattr") << ::boost::errinfo_file_name(__filename)
  //            << ::boost::errinfo_errno(errno);
  //      }
  //
  //      activity_base::start();
  //    }
  //
  //    void serial_port::stop() {
  //      activity_base::stop();
  //
  //      try {
  //        if (__stream_ptr && __stream_ptr->is_open()) {
  //          __stream_ptr->close();
  //        }
  //      } catch (...) {
  //      }
  //
  //      __stream_ptr.reset();
  //    }

  } // namespace activity
} // namespace anyterm

