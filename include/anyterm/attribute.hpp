/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#ifndef __ANYTERM_ATTRIBUTE_HPP__
#define __ANYTERM_ATTRIBUTE_HPP__

#include <cstdint>
#include <string>

namespace anyterm {

  struct attribute {
    public:
      attribute();

      static char const default_foreground[];
      static char const default_background[];
      static char const default_bold[];
      static char const default_faint[];
      static char const default_bright[];
      static char const default_cursor[];
      static char const unkonwn[];

      bool operator==(attribute const& o) const;
      bool operator!=(attribute const& o) const;
      bool operator<(attribute const& o) const;
      bool operator<=(attribute const& o) const;
      bool operator>(attribute const& o) const;
      bool operator>=(attribute const& o) const;

      void set_row(std::uint32_t const row);
      void set_column(std::uint32_t const column);
      void set_foreground(std::string const& foreground);
      void set_background(std::string const& foreground);
      void set_underlined(bool const underlined);
      void set_strikethrough(bool const strikethrough);
      void set_cursor(bool const cursor);
      void set_bold(bool const bold);
      void set_faint(bool const faint);
      void set_bright(bool const bright);
      void set_blink(bool const blink);

      std::uint32_t get_row() const;
      std::uint32_t get_column() const;

      std::string to_css() const;

    private:
      std::uint32_t row;
      std::uint32_t column;
      std::string   foreground;
      std::string   background;
      bool          underlined;
      bool          strikethrough;
      bool          cursor;
      bool          bold;
      bool          faint;
      bool          bright;
      bool          blink;
  };

} // namespace anyterm

#endif // ifndef __ANYTERM_ATTRIBUTE_HPP__
