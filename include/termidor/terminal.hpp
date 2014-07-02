/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#ifndef __TERMIDOR_TERMINAL_HPP__
#define __TERMIDOR_TERMINAL_HPP__

#include <cstdint>
#include <string>

#include "termidor/screen.hpp"

namespace termidor {

  class terminal {
    public:
      terminal();
      ~terminal();

      void set_size(std::uint16_t const row_count, std::uint16_t const column_count);
      void set_scrollback_size(std::int16_t const scrollback_size);

      std::uint32_t column_count();
      std::uint32_t row_count();

      std::uint32_t cursor_column();
      std::uint32_t cursor_row();

      void login(std::string const& username);

      void   write(std::string const& data);
      screen read();

      bool is_alive() const;
      void set_alive(bool const alive);

      bool is_dirty() const;
      void set_dirty(bool const dirty);

    private:
      bool volatile alive;
      bool volatile dirty;

      void* widget;
  };

} // namespace termidor

#endif // ifndef __TERMIDOR_TERMINAL_HPP__
