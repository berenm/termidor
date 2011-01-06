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

  //    struct serial_port: virtual public activity_base {
  //        serial_port(activity_base::on_output_slot_t on_output_in,
  //                    activity_base::on_error_slot_t on_error_in,
  //                    ::std::string const& filename_in,
  //                    ::std::uint32_t const baudrate_in);
  //        virtual ~serial_port();
  //
  //        virtual void start();
  //        virtual void stop();
  //
  //      protected:
  //        ::std::string const __filename;
  //        ::std::uint32_t const __baudrate;
  //    };
  //
  } // namespace activity
//
//  struct serial_port_factory: public activity_factory_base {
//      serial_port_factory(::std::string const& filename_in, ::std::uint32_t const baudrate_in) :
//        __file_name(filename_in), __baudrate(baudrate_in) {
//      }
//
//      activity_base* operator()(activity_base::on_output_slot_t on_output_in,
//                                activity_base::on_error_slot_t on_error_in) const {
//        return new activity::serial_port(on_output_in, on_error_in, __file_name, __baudrate);
//      }
//
//    protected:
//      ::std::string const __file_name;
//      ::std::uint32_t const __baudrate;
//  };

} // namespace anyterm

#endif /* ANYTERM_ACTIVITY_SERIAL_PORT_HPP_ */
