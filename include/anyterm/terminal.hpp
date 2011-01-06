/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_TERMINAL_HPP_
#define ANYTERM_TERMINAL_HPP_

#include <vte/vte.h>

#include "anyterm/attribute.hpp"
#include "anyterm/screen.hpp"

#include <string>

namespace anyterm {

  class terminal {
    public:
      terminal(::std::uint8_t const row_count_in,
               ::std::uint8_t const column_count_in,
               ::std::uint16_t const scrollback_count_in);
      ~terminal();

      void set_size(::std::uint8_t const row_count_in, ::std::uint8_t const column_count_in);
      void set_scrollback_size(::std::int16_t const scrollback_size_in);

      ::std::uint32_t column_count();
      ::std::uint32_t row_count();

      ::std::uint32_t cursor_column();
      ::std::uint32_t cursor_row();

      void fork_command(::std::string const& command_in);

      void write(::std::string const& string_in);
      screen read();

    private:
      GtkWidget* __terminal;
  };

} // namespace anyterm

#endif /* ANYTERM_TERMINAL_HPP_ */
