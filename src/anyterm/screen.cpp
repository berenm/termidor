/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#include "anyterm/screen.hpp"

#include <algorithm>

namespace anyterm {

  screen::screen(text_lines const& lines, text_attributes const& attributes) :
    lines(lines), attributes(attributes) {
    std::sort(this->attributes.begin(), this->attributes.end());
  }

  screen::~screen() {}

  screen::text_lines screen::get_lines() const { return this->lines; }
  screen::text_attributes screen::get_attributes() const { return this->attributes; }

  anyterm::attribute screen::get_attribute(std::uint32_t const row, std::uint32_t const column) const {
    if (this->attributes.size() < row + 1)
      return anyterm::attribute();

    screen::line_attributes const& line_attribs = this->attributes[row];
    if (line_attribs.size() < column + 1)
      return anyterm::attribute();

    anyterm::attribute attrib = line_attribs[column];
    attrib.set_row(row);
    attrib.set_column(column);

    return attrib;
  }

  void screen::set_attribute(std::uint32_t const row, std::uint32_t const column, anyterm::attribute const& attribute) {
    anyterm::attribute attrib = attribute;

    attrib.set_row(row);
    attrib.set_column(column);

    if (this->attributes.size() < row + 1)
      this->attributes.resize(row + 1, screen::line_attributes());

    screen::line_attributes& line_attribs = this->attributes[row];
    if (line_attribs.size() < column + 1)
      line_attribs.resize(column + 1, anyterm::attribute());

    line_attribs[column] = attrib;
    if (lines.size() < row + 1)
      lines.resize(row + 1, std::wstring(column, L' '));

    std::wstring& line = lines[row];
    if (line.size() < column)
      line.append(std::wstring(column - line.size(), L' '));
  }

} // namespace anyterm
