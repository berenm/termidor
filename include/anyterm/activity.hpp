/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_ACTIVITY_HPP_
#define ANYTERM_ACTIVITY_HPP_

#include <string>

#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/asio.hpp>

namespace bio = ::boost::iostreams;
namespace asio = ::boost::asio;
namespace asiop = ::boost::asio::posix;

namespace anyterm {

  struct activity_base {
    public:
      static ::asio::io_service io_service_;

      typedef ::boost::signals2::signal_type< void(::std::string const&) > signal_t;
      typedef signal_t::slot_function_type on_output_slot_t;
      typedef signal_t::slot_function_type on_error_slot_t;

      activity_base(on_output_slot_t on_output_in, on_error_slot_t on_error_in);
      virtual ~activity_base();

      void send(::std::string const& string_in);
      void process_output();

      bool is_good();
      bool is_started();
      bool has_output();

      virtual void start();
      virtual void stop();

    protected:
      ::boost::mutex __mutex;

      signal_t::type __on_output;
      signal_t::type __on_error;

      ::boost::shared_ptr< ::asiop::stream_descriptor > __stream_ptr;
      volatile bool __started;
  };

  struct activity_factory_base {
      virtual activity_base* operator()(activity_base::on_output_slot_t on_output_in,
                                        activity_base::on_error_slot_t on_error_in) const = 0;
  };

} // namespace anyterm

#endif /* ANYTERM_ACTIVITY_HPP_ */
