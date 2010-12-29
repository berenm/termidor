/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_ACTIVITY_SERIAL_PORT_HPP_
#define ANYTERM_ACTIVITY_SERIAL_PORT_HPP_

#include "anyterm/activity.hpp"

#include <string>

namespace anyterm {
  namespace activity {

    // This base class is just so that sp can be initialised before being Activity.
    struct serial_port_base {
        serial_port_base(::std::string const& filename_in,
                         ::std::ios::openmode const open_mode_in,
                         ::std::uint32_t const baudrate_in,
                         bool const raw_in = true);
        void set_serial_mode(::std::string const& filename_in,
                             ::std::uint32_t const baudrate_in,
                             bool const raw_in);

      protected:
        ::boost::shared_ptr< ::asiop::stream_descriptor > __file_stream_ptr;
    };

    class serial_port: private serial_port_base, public activity_base {
      public:
        serial_port(activity_base::on_output_slot_t on_output_in,
                    activity_base::on_error_slot_t on_error_in,
                    ::std::string const& filename_in,
                    ::std::uint32_t const baudrate_in);
        ~serial_port() {
        }
    };

  } // namespace activity

  struct serial_port_factory: public activity_factory_base {
      serial_port_factory(::std::string const& filename_in, ::std::uint32_t const baudrate_in) :
        __file_name(filename_in), __baudrate(baudrate_in) {
      }

      activity_base* operator()(activity_base::on_output_slot_t on_output_in,
                                activity_base::on_error_slot_t on_error_in) const {
        return new activity::serial_port(on_output_in, on_error_in, __file_name, __baudrate);
      }

    protected:
      ::std::string const __file_name;
      ::std::uint32_t const __baudrate;
  };

} // namespace anyterm

#endif /* ANYTERM_ACTIVITY_SERIAL_PORT_HPP_ */
