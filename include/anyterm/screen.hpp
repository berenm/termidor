/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#ifndef __ANYTERM_SCREEN_HPP__
#define __ANYTERM_SCREEN_HPP__

#include <cstdint>
#include <string>
#include <vector>

#include "anyterm/attribute.hpp"

namespace anyterm {

  struct screen {
    public:
      typedef std::vector< std::wstring >    text_lines;
      typedef std::vector< attribute >       line_attributes;
      typedef std::vector< line_attributes > text_attributes;

      screen(text_lines const& lines, text_attributes const& attributes=text_attributes());
      ~screen();

      text_lines      get_lines() const;
      text_attributes get_attributes() const;
      std::string     to_html() const;

      anyterm::attribute get_attribute(std::uint32_t const row, std::uint32_t const column) const;
      void               set_attribute(std::uint32_t const row, std::uint32_t const column, anyterm::attribute const& attribute);

    private:
      text_lines      lines;
      text_attributes attributes;
  };

} // namespace anyterm

#endif // ifndef __ANYTERM_SCREEN_HPP__
