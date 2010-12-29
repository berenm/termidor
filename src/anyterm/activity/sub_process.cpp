/**
 * @file
 * @date Dec 29, 2010
 */

#include "anyterm/activity/sub_process.hpp"
#include "anyterm/error.hpp"

#include <iostream>

// For forkpty():
#if defined(__FreeBSD__)
#include <libutil.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(__OpenBSD__) || defined(__NetBSD__)
#include <termios.h>
#include <util.h>
#elif defined(__APPLE__)
#include <util.h>
#elif defined(__sun__)
#include "sun_forkpty.h"
#include <sys/tty.h>
#else
#include <pty.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include <boost/bind.hpp>

namespace anyterm {
  namespace activity {

    static ::std::pair< ::boost::shared_ptr< ::asiop::stream_descriptor >, ::pid_t > open_subprocess(::std::string const& command_in,
                                                                                                     ::std::uint8_t const terminal_row_count_in,
                                                                                                     ::std::uint8_t const terminal_column_count_in) {
      struct winsize terminal_size;
      terminal_size.ws_row = terminal_row_count_in;
      terminal_size.ws_col = terminal_column_count_in;
      terminal_size.ws_xpixel = 0;
      terminal_size.ws_ypixel = 0;

      int handle;
      int process_id = forkpty(&handle, NULL, NULL, &terminal_size);
      if (process_id == -1) {
        throw error() << ::boost::errinfo_api_function("forkpty") << ::boost::errinfo_errno(errno);
      }

      if (process_id == 0) {
        setenv("TERM", "linux", 1);
        struct termios t;

        // Could fail, but where would we send the error?
        tcgetattr(0, &t);

        // Make ctrl-H (backspace) the erase character.
        t.c_cc[VERASE] = 8;
        tcsetattr(0, TCSANOW, &t);

        execl("/bin/sh", "/bin/sh", "-c", "su username -", NULL);

        throw error() << ::boost::errinfo_api_function("execl") << ::boost::errinfo_errno(errno);
      }

      return ::std::make_pair(new ::asiop::stream_descriptor(activity_base::io_service_, handle), process_id);
    }

    sub_process_base::sub_process_base(::std::pair< ::boost::shared_ptr< ::asiop::stream_descriptor >,
        ::pid_t > const& filedescriptor_process_id_in) :
      __file_stream_ptr(filedescriptor_process_id_in.first),
          __process_id(filedescriptor_process_id_in.second) {
    }

    sub_process_base::~sub_process_base() {
      if (__file_stream_ptr->is_open()) {
        __file_stream_ptr->close();
      }
    }

    sub_process::sub_process(activity_base::on_output_slot_t on_output_in,
                             activity_base::on_error_slot_t on_error_in,
                             ::std::string const& command_in,
                             ::std::uint8_t const terminal_row_count_in,
                             ::std::uint8_t const terminal_column_count_in) :
      sub_process_base(open_subprocess(command_in, terminal_row_count_in, terminal_column_count_in)),
          activity_base(on_output_in, on_error_in, sub_process_base::__file_stream_ptr) {
      ::std::clog << "sub_process()" << ::std::endl;
    }

    sub_process::~sub_process() {
      ::std::clog << "~sub_process()" << ::std::endl;
      // We do two things to try to kill the subprocess: we close the fd, which really ought to kill it, and we SIGHUP it.
      // The SIGHUP by itself may not be sufficient if the process is nohup or setuid or something.
      // The close by itself really should be sufficient, but I'm keeping the SIGHUP because I'm paranoid.
      // The three results that we want are
      //   (a) the process dies,
      //   (b) it does not become a zombie,
      //   (c) the output processor thread terminates so that ~Activity can join it.
      // For (c), we hope that it will get an error when reading from the fd and/or that it sees terminating set.
      // There's a danger that we could close the fd and something else could open another fd with the same number, which the output processor could read.
      // I hope that's not a high probability.

      __terminating = true;

      kill(__process_id, SIGHUP);
      waitpid(__process_id, 0, 0);
      try {
        sub_process_base::__file_stream_ptr->close();
      } catch (...) {
      }
    }

  } // namespace activity
} // namespace anyterm

