/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#include "anyterm/attribute.hpp"

namespace anyterm {

  char const attribute::default_foreground[] = "d";
  char const attribute::default_background[] = "D";
  char const attribute::default_bold[]       = "db";
  char const attribute::default_faint[]      = "dh";
  char const attribute::default_bright[]     = "di";
  char const attribute::default_cursor[]     = "dc";
  char const attribute::unkonwn[]            = "u";

#ifdef ANYTERM_BUILD_DEBUG
  attribute::attribute() :
    foreground(attribute::default_foreground),
    background(attribute::default_background),
    underlined(false),
    strikethrough(false),
    cursor(false),
    bold(false),
    faint(false),
    bright(false),
    blink(false)
  {}

#else // ifdef ANYTERM_BUILD_DEBUG
  attribute::attribute() :
    foreground(attribute::default_foreground),
    background(attribute::default_background),
    underlined(false),
    strikethrough(false),
    cursor(false),
    bold(false),
    faint(false),
    bright(false),
    blink(false)
  {}

#endif // ifdef ANYTERM_BUILD_DEBUG

  bool attribute::operator==(attribute const& o) const {
    return this->foreground == o.foreground
           && this->background == o.background
           && underlined == o.underlined
           && strikethrough == o.strikethrough
           && cursor == o.cursor
           && bold == o.bold
           && faint == o.faint
           && bright == o.bright
           && blink == o.blink;
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

  void attribute::set_bold(bool const bold) {
    this->bold = bold;
  }

  void attribute::set_faint(bool const faint) {
    this->faint = faint;
  }

  void attribute::set_bright(bool const bright) {
    this->bright = bright;
  }

  void attribute::set_blink(bool const blink) {
    this->blink = blink;
  }

  std::uint32_t attribute::get_row() const {
    return this->row;
  }

  std::uint32_t attribute::get_column() const {
    return this->column;
  }

  std::string attribute::to_css() const {
    std::string css_classes;

#ifdef ANYTERM_BUILD_DEBUG
    if (this->foreground != attribute::default_foreground)
      css_classes += "foreground_" + this->foreground + " ";
    if (this->background != attribute::default_background)
      css_classes += "background_" + this->background + " ";
    if (this->underlined)
      css_classes += "underlined_yes ";
    if (this->strikethrough)
      css_classes += "strikethrough_yes ";
    if (this->cursor)
      css_classes += "cursor_yes ";
    if (this->bold)
      css_classes += "bold_yes ";
    if (this->faint)
      css_classes += "faint_yes ";
    if (this->bright)
      css_classes += "bright_yes ";
    if (this->blink)
      css_classes += "blink_yes ";
#else // ifdef ANYTERM_BUILD_DEBUG
    if (this->foreground != attribute::default_foreground)
      css_classes += "f" + this->foreground + " ";
    if (this->background != attribute::default_background)
      css_classes += "b" + this->background + " ";
    if (this->underlined)
      css_classes += "uy ";
    if (this->strikethrough)
      css_classes += "sy ";
    if (this->cursor)
      css_classes += "cy ";
    if (this->bold)
      css_classes += "by ";
    if (this->faint)
      css_classes += "hy ";
    if (this->bright)
      css_classes += "iy ";
    if (this->blink)
      css_classes += "ly ";
#endif // ifdef ANYTERM_BUILD_DEBUG

    return css_classes;
  } // to_css

} // namespace anyterm
