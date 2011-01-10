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
      terminal();
      ~terminal();

      void set_size(::std::uint16_t const row_count_in, ::std::uint16_t const column_count_in);
      void set_scrollback_size(::std::int16_t const scrollback_size_in);

      ::std::uint32_t column_count();
      ::std::uint32_t row_count();

      ::std::uint32_t cursor_column();
      ::std::uint32_t cursor_row();

      void login(::std::string const& username_in = "remi");

      void write(::std::string const& data_in);
      screen read();

      bool is_alive() const;
      void set_alive(bool const alive_in);

      bool is_dirty() const;
      void set_dirty(bool const dirty_in);

    private:
      volatile bool __alive;
      volatile bool __dirty;

      GtkWidget* __terminal;
  };

} // namespace anyterm

#endif /* ANYTERM_TERMINAL_HPP_ */
