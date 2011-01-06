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

    sub_process::sub_process(activity_base::on_output_slot_t on_output_in,
                             activity_base::on_error_slot_t on_error_in,
                             ::std::string const& command_in,
                             ::std::uint8_t const terminal_row_count_in,
                             ::std::uint8_t const terminal_column_count_in) :
      activity_base(on_output_in, on_error_in), __process_id(-1), __command(command_in),
          __terminal_row_count(terminal_row_count_in), __terminal_column_count(terminal_column_count_in) {
      ::std::clog << "sub_process()" << ::std::endl;
    }

    sub_process::~sub_process() {
      ::std::clog << "~sub_process()" << ::std::endl;
      stop();
    }

    void sub_process::start() {
      if (__process_id != -1) {
        ::std::clog << "sub_process_base: process already started." << ::std::endl;
        return;
      }

      struct winsize terminal_size;
      terminal_size.ws_row = __terminal_row_count;
      terminal_size.ws_col = __terminal_column_count;
      terminal_size.ws_xpixel = 0;
      terminal_size.ws_ypixel = 0;

      __terminal = vte_terminal_new();
      char* args[] = { "/bin/sh", "-c", const_cast< char* > (__command.c_str()), NULL };
      //      vte_terminal_fork_command(VTE_TERMINAL(__terminal), "/bin/sh", args, NULL, "/", false, false, false);

      int handle;
      int process_id = vte_terminal_fork_command(VTE_TERMINAL(__terminal),
                                                 "/bin/sh",
                                                 args,
                                                 NULL,
                                                 "/",
                                                 false,
                                                 false,
                                                 false);
      if (process_id == -1) {
        throw error() << ::boost::errinfo_api_function("forkpty") << ::boost::errinfo_errno(errno);
      }

      handle = vte_terminal_get_pty(VTE_TERMINAL(__terminal));

      //      if (process_id == 0) {
      //        setenv("TERM", "xterm", 1);
      //        struct termios t;
      //
      //        // Could fail, but where would we send the error?
      //        tcgetattr(0, &t);
      //
      //        // Make ctrl-H (backspace) the erase character.
      //        t.c_cc[VERASE] = 8;
      //        tcsetattr(0, TCSANOW, &t);
      //
      //        execl("/bin/sh", "/bin/sh", "-c", __command.c_str(), NULL);
      //
      //        throw error() << ::boost::errinfo_api_function("execl") << ::boost::errinfo_errno(errno);
      //      }

      __stream_ptr.reset(new ::asiop::stream_descriptor(activity_base::io_service_, handle));
      ::asiop::descriptor_base::non_blocking_io non_blocking_io_command;
      __stream_ptr->io_control(non_blocking_io_command);

      __process_id = process_id;

      activity_base::start();
    }

    void sub_process::stop() {
      activity_base::stop();

      if (__process_id == -1) {
        return;
      }

      gtk_widget_destroy(__terminal);

      kill(__process_id, SIGHUP);
      waitpid(__process_id, 0, 0);
      try {
        if (__stream_ptr && __stream_ptr->is_open()) {
          __stream_ptr->close();
        }
      } catch (...) {
      }

      __process_id = -1;
      __stream_ptr.reset();
    }

  } // namespace activity
} // namespace anyterm

