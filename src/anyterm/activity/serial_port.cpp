/**
 * @file
 * @date Dec 29, 2010
 */

#include "anyterm/activity/serial_port.hpp"
#include "anyterm/error.hpp"

#include <termios.h>
#include <unistd.h>

namespace anyterm {
  namespace activity {
    serial_port_base::serial_port_base(::std::string const& filename_in,
                                       ::std::ios::openmode const open_mode_in,
                                       ::std::uint32_t const baudrate_in,
                                       bool const raw_in) :
      __file_stream_ptr(new ::asiop::stream_descriptor(activity_base::io_service_,
                                                       ::bio::file_descriptor(filename_in).handle())) {
      set_serial_mode(filename_in, baudrate_in, raw_in);
    }

    void serial_port_base::set_serial_mode(::std::string const& filename_in,
                                           ::std::uint32_t const baudrate_in,
                                           bool const raw_in) {
      // Quote from the glibc docs:
      //   In the GNU library, the functions [cfsetispeed, cfsetospeed, etc] accept
      //   speeds measured in bits per second as input, and return speed values
      //   measured in bits per second. Other libraries require speeds to be indicated
      //   by special codes.
      // So this code will compile, but probably not work, on non-glibc systems.

      struct termios settings;
      int rc = tcgetattr(__file_stream_ptr->native(), &settings);
      if (rc < 0) {
        throw error() << ::boost::errinfo_api_function("tcgetattr")
            << ::boost::errinfo_file_name(filename_in) << ::boost::errinfo_errno(errno);
      }

      if (raw_in) {
        // The following is equivalent to the non-portable cfmakeraw(&settings)
        settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
        settings.c_oflag &= ~OPOST;
        settings.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        settings.c_cflag &= ~(CSIZE | PARENB);
        settings.c_cflag |= CS8;
      }

      rc = cfsetospeed(&settings, baudrate_in);
      if (rc != 0) {
        throw error() << ::boost::errinfo_api_function("cfsetospeed")
            << ::boost::errinfo_file_name(filename_in) << ::boost::errinfo_errno(errno);
      }

      rc = cfsetispeed(&settings, baudrate_in);
      if (rc != 0) {
        throw error() << ::boost::errinfo_api_function("cfsetispeed")
            << ::boost::errinfo_file_name(filename_in) << ::boost::errinfo_errno(errno);
      }

      rc = tcsetattr(__file_stream_ptr->native(), TCSANOW, &settings);
      if (rc < 0) {
        throw error() << ::boost::errinfo_api_function("tcsetattr")
            << ::boost::errinfo_file_name(filename_in) << ::boost::errinfo_errno(errno);
      }
    }

    serial_port::serial_port(activity_base::on_output_slot_t on_output_in,
                             activity_base::on_error_slot_t on_error_in,
                             ::std::string const& filename_in,
                             ::std::uint32_t const baudrate_in) :
      serial_port_base(filename_in, ::std::ios::in | ::std::ios::out, baudrate_in),
          activity_base(on_output_in, on_error_in, serial_port_base::__file_stream_ptr) {
    }

  } // namespace activity
} // namespace anyterm

