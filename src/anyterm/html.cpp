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
#include <sstream>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/regex/pending/unicode_iterator.hpp>

struct row_transformer {
    row_transformer(::std::stringstream& stream_inout,
                    ::anyterm::screen const& screen_in,
                    ::std::uint32_t const row_number_in,
                    ::anyterm::attribute& last_attribute_in) :
      __stream(stream_inout), __screen(screen_in), __row_number(row_number_in), __column_number(0),
          __last_attribute(last_attribute_in) {
    }

    void operator()(wchar_t const& char_in) {
      ::anyterm::attribute attribute = __screen.get_attribute(__row_number, __column_number);
      if (attribute != __last_attribute) {
        if (__last_attribute != ::anyterm::attribute()) {
          __stream << "</span>";
        }

        if (attribute != ::anyterm::attribute()) {
          __stream << "<span class='" << attribute.to_css() << "'>";
        }

        __last_attribute = attribute;
      }

      typedef ::boost::u32_to_u8_iterator< ::std::wstring::iterator > to_u8;
      ::std::wstring wstr(1, char_in);

      switch (char_in) {
        case '<':
          __stream << "&lt;";
          break;
        case '>':
          __stream << "&gt;";
          break;
        case '&':
          __stream << "&amp;";
          break;
        case '\n':
          __stream << "<br/>";
          break;
        default:
          __stream << ::std::string(to_u8(wstr.begin()), to_u8(wstr.end()));
          break;
      }

      __column_number++;
    }

    ::std::string str() {
      return ::boost::trim_right_copy(__stream.str());
    }

    ::std::uint32_t column_number() {
      return __column_number;
    }

  private:
    ::std::stringstream& __stream;
    ::anyterm::screen const& __screen;
    ::std::uint32_t const __row_number;
    ::std::uint32_t __column_number;
    ::anyterm::attribute& __last_attribute;
};

struct lines_transformer {
    lines_transformer(::std::stringstream& stream_inout, ::anyterm::screen const& screen_in) :
      __stream(stream_inout), __screen(screen_in), __row_number(0), __last_attribute() {
    }

    void operator()(::std::wstring const& row_in) {
      ::std::stringstream stream;
      ::std::for_each(row_in.begin(), row_in.end(), row_transformer(stream,
                                                                    __screen,
                                                                    __row_number,
                                                                    __last_attribute));
      __stream << ::boost::trim_right_copy_if(stream.str(), ::boost::is_any_of(" \t")) << "<br/>";

      __row_number++;
    }

  private:
    ::std::stringstream& __stream;
    ::anyterm::screen const& __screen;
    ::std::uint32_t __row_number;
    ::anyterm::attribute __last_attribute;
};

::std::string htmlify_screen(::anyterm::screen const& screen_in) {
  ::anyterm::screen::line_vector_t const& lines = screen_in.lines();

  ::std::stringstream html_stream;
  html_stream << "<span class='" << ::anyterm::attribute().to_css() << "'>";

  ::std::for_each(lines.begin(), lines.end(), lines_transformer(html_stream, screen_in));

  return ::boost::trim_right_copy_if(html_stream.str(), ::boost::is_any_of(" \n")) + "</span>";
}

