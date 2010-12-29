/**
 * @file
 * @date Dec 29, 2010
 */

#ifndef ANYTERM_SESSION_HPP_
#define ANYTERM_SESSION_HPP_

#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "anyterm/screen.hpp"
#include "anyterm/terminal.hpp"
#include "anyterm/activity.hpp"
//#include "Iconver.hh"

namespace anyterm {

  class session: ::boost::noncopyable {
    public:

      //      const SessionId id;
      ::std::uint8_t const row_count;
      ::std::uint8_t const column_count;
      ::std::uint16_t const scrollback_count;
      ::std::uint32_t const timeout;
      ::std::string const charset;
      bool const diff;

    private:
      //      pbe::Iconver< pbe::permissive, utf8_char, char > utf8_to_charset;
      //      pbe::Iconver< pbe::permissive, char, ucs4_char > charset_to_ucs4;
      //      pbe::Iconver< pbe::valid, ucs4_char, utf8_char > ucs4_to_utf8;

    public:
      screen screen_;
      ::boost::mutex screen_mutex;

      volatile bool dirty;
      ::boost::condition_variable dirty_condition;

      volatile bool error;
      ::std::string error_message;

      ::std::wstring old_html_screen;
      volatile time_t last_access;

      terminal __terminal;
      ::boost::scoped_ptr< activity_base > activity;

      session(::std::uint8_t const row_count_in,
              ::std::uint8_t const column_count_in,
              ::std::uint16_t const scrollback_count_in,
              ::std::uint32_t const timeout_in,
              activity_factory_base const& activity_factory_in,
              ::std::string const& charset_in,
              bool const diff_in);
      ~session();

      void touch(void);
      void report_any_backend_error(void);
      void send(::std::string k);
      ::std::string rcv();

      bool timed_out(void);

    private:
      void process_output(::std::string const& string_in);
      void process_error(::std::string const& string_in);
  };

} // namespace anyterm

#endif /* ANYTERM_SESSION_HPP_ */
