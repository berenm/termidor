// common/html.cc
// This file is part of Anyterm; see http://anyterm.org/
// (C) 2005 Philip Endecott

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#include "anyterm/html.hpp"

#include <string>

// Screen to HTML conversion:

static bool gen_style(ucs4_string& html_out, ::anyterm::attributes const& attributes_in) {
  if (attributes_in != ::anyterm::attributes()) {
    ::std::uint8_t foreground = attributes_in.foreground_;
    ::std::uint8_t background = attributes_in.background_;

    if (attributes_in.inverse_) {
      ::std::swap(foreground, background);
    }

    ucs4_string classes;
    if (attributes_in.bold_) {
      classes += L'z';
    }

    if (background != ::anyterm::attributes().background_) {
      if (!classes.empty()) {
        classes += L' ';
      }
      classes += L'a' + background;
    }

    if (foreground != ::anyterm::attributes().foreground_) {
      if (!classes.empty()) {
        classes += L' ';
      }
      classes += L'i' + foreground;
    }

    html_out += L"<span class=\"" + classes + L"\">";

    return true;
  }

  return false;
}

static const ucs4_char* attr_end = L"</span>";

static const ucs4_char* cursor_start = L"<span class=\"cursor\">";
static const ucs4_char* cursor_end = L"</span>";

ucs4_string htmlify_screen(const ::anyterm::screen& screen) {
  // Convert screen into HTML.
  // Slightly optimised to reduce spaces at right end of lines.

  ucs4_string h;

  for (int row = -screen.scrollback(); row < screen.rows(); row++) {
    int sp = 0;
    bool styled = false;
    ::anyterm::attributes prev_attrs;
    for (int column = 0; column < screen.cols(); column++) {
      bool cursor = (row == screen.cursor_row_number_ && column == screen.cursor_column_number_)
          && screen.cursor_visible_;
      ::anyterm::cell cell = screen(row, column);
      ucs4_char ch = cell.char_;
      ::anyterm::attributes attrs = cell.attributes_;

      if (ch == ' ' && attrs == ::anyterm::attributes() && !styled && column > 0 && row > 0 && !cursor) {
        sp++;
      } else {
        while (sp > 0) {
          h += L'\u00A0';
          sp--;
        }
        if (styled && attrs != prev_attrs) {
          h += attr_end;
        }
        if (column == 0 || attrs != prev_attrs) {
          styled = gen_style(h, attrs);
          prev_attrs = attrs;
        }
        if (cursor) {
          h += cursor_start;
        }
        switch (ch) {
          case '<':
            h += L"&lt;";
            break;
          case '>':
            h += L"&gt;";
            break;
          case '&':
            h += L"&amp;";
            break;
          case ' ':
            h += L'\u00A0';
            break;
          default:
            h += ch;
            break;
        }
        if (cursor) {
          h += cursor_end;
        }
      }
    }
    if (styled) {
      h += attr_end;
    }
    h += L"<br>";
  }

  return h;
}

