/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_ACTIVITY_SUB_PROCESS_HPP_
#define ANYTERM_ACTIVITY_SUB_PROCESS_HPP_

#include <vte/vte.h>

#include "anyterm/activity.hpp"

#include <string>

namespace anyterm {
  namespace activity {

    struct sub_process: virtual public activity_base {
        sub_process(activity_base::on_output_slot_t on_output_in,
                    activity_base::on_error_slot_t on_error_in,
                    ::std::string const& command_in,
                    ::std::uint8_t const terminal_row_count_in = 25,
                    ::std::uint8_t const terminal_column_count_in = 80);
        virtual ~sub_process();

        virtual void start();
        virtual void stop();

      protected:
        GtkWidget* __terminal;

        ::pid_t __process_id;
        ::std::string const __command;
        ::std::uint8_t const __terminal_row_count;
        ::std::uint8_t const __terminal_column_count;
    };

  } // namespace activity

  struct sub_process_factory: public activity_factory_base {
      sub_process_factory(::std::string const& command_in,
                          ::std::uint8_t const terminal_row_count_in = 25,
                          ::std::uint8_t const terminal_column_count_in = 80) :
        __command(command_in), __terminal_row_count(terminal_row_count_in),
            __terminal_column_count(terminal_column_count_in) {
      }

      activity_base* operator()(activity_base::on_output_slot_t on_output_in,
                                activity_base::on_error_slot_t on_error_in) const {
        return new activity::sub_process(on_output_in,
                                         on_error_in,
                                         __command,
                                         __terminal_row_count,
                                         __terminal_column_count);
      }

    protected:
      ::std::string const __command;
      ::std::uint8_t const __terminal_row_count;
      ::std::uint8_t const __terminal_column_count;
  };

} // namespace anyterm

#endif /* ANYTERM_ACTIVITY_SUB_PROCESS_HPP_ */
