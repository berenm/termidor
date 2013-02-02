/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#include "anyterm/attribute.hpp"

namespace anyterm {

#ifdef ANYTERM_BUILD_DEBUG
  attribute::attribute() :
    foreground("def_fg"),
    background("def_bg"),
    underlined(false),
    strikethrough(false),
    cursor(false)

    // , halfbright(false)
    // , bold(false)
    // , blink(false)
    // , inverse(false)
  {}

#else // ifdef ANYTERM_BUILD_DEBUG
  attribute::attribute() :
    foreground("d"),
    background("d"),
    underlined(false),
    strikethrough(false),
    cursor(false)

    // , halfbright(false)
    // , bold(false)
    // , blink(false)
    // , inverse(false)
  {}

#endif // ifdef ANYTERM_BUILD_DEBUG

  bool attribute::operator==(attribute const& o) const {
    return this->foreground == o.foreground
           && this->background == o.background
           && underlined == o.underlined
           && strikethrough == o.strikethrough
           && cursor == o.cursor;

    // && halfbright == o.halfbright
    // && bold == o.bold
    // && blink == o.blink
    // && inverse == o.inverse
  }

  bool attribute::operator!=(attribute const& o) const {
    return !operator==(o);
  }

  bool attribute::operator<(attribute const& o) const {
    return this->row < o.row || (this->row == o.row && this->column < o.column);
  }

  bool attribute::operator<=(attribute const& o) const {
    return this->row < o.row || (this->row == o.row && this->column <= o.column);
  }

  bool attribute::operator>(attribute const& o) const {
    return this->row > o.row || (this->row == o.row && this->column > o.column);
  }

  bool attribute::operator>=(attribute const& o) const {
    return this->row > o.row || (this->row == o.row && this->column >= o.column);
  }

  void attribute::set_row(std::uint32_t const row) {
    this->row = row;
  }

  void attribute::set_column(std::uint32_t const column) {
    this->column = column;
  }

  void attribute::set_foreground(std::string const& foreground) {
    this->foreground = foreground;
  }

  void attribute::set_background(std::string const& background) {
    this->background = background;
  }

  void attribute::set_underlined(bool const underlined) {
    this->underlined = underlined;
  }

  void attribute::set_strikethrough(bool const strikethrough) {
    this->strikethrough = strikethrough;
  }

  void attribute::set_cursor(bool const cursor) {
    this->cursor = cursor;
  }

  // void attribute::set_halfbright(bool const halfbright) {
  // this->halfbright = halfbright;
  // }
  // void attribute::set_bold(bool const bold) {
  // this->bold = bold;
  // }
  // void attribute::set_blink(bool const blink) {
  // this->blink = blink;
  // }
  // void attribute::setverse(bool const inverse) {
  // this->inverse = inverse;
  // }
  // void attribute::set_cursor(bool const cursor) {
  // this->cursor = cursor;
  // }

  std::uint32_t attribute::get_row() const {
    return this->row;
  }

  std::uint32_t attribute::get_column() const {
    return this->column;
  }

  std::string attribute::to_css() const {
    std::string css_classes;

#ifdef ANYTERM_BUILD_DEBUG
    css_classes += "attributes ";
    css_classes += "foreground_" + this->foreground + " ";
    css_classes += "background_" + this->background + " ";
    css_classes += std::string("underlined_") + (this->underlined ? "yes" : "no") + " ";
    css_classes += std::string("strikethrough_") + (this->strikethrough ? "yes" : "no") + " ";
    css_classes += std::string("cursor_") + (this->cursor ? "yes" : "no") + " ";

    // css_classes += std::string("halfbright_") + (this->halfbright ? "yes" : "no");
    // css_classes += std::string("bold_") + (this->bold ? "yes" : "no");
    // css_classes += std::string("blink_") + (this->blink ? "yes" : "no");
    // css_classes += std::string("inverse_") + (this->inverse ? "yes" : "no");
#else // ifdef ANYTERM_BUILD_DEBUG
    css_classes += "a ";
    css_classes += "f" + this->foreground + " ";
    css_classes += "b" + this->background + " ";
    css_classes += std::string("u") + (this->underlined ? "y" : "n") + " ";
    css_classes += std::string("s") + (this->strikethrough ? "y" : "n") + " ";
    css_classes += std::string("c") + (this->cursor ? "y" : "n") + " ";

    // css_classes += std::string("h") + (this->halfbright ? "y" : "n");
    // css_classes += std::string("b") + (this->bold ? "y" : "n");
    // css_classes += std::string("l") + (this->blink ? "y" : "n");
    // css_classes += std::string("i") + (this->inverse ? "y" : "n");
#endif // ifdef ANYTERM_BUILD_DEBUG

    return css_classes;
  }

} // namespace anyterm
