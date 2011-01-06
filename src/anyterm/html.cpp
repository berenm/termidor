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

::anyterm::attribute get_attribute(::anyterm::screen::attribute_vector_t const& attributes_in,
                                   ::std::uint32_t row_number_in,
                                   ::std::uint32_t column_number_in) {
  ::anyterm::attribute attribute_out;
  attribute_out.set_row(row_number_in);
  attribute_out.set_column(column_number_in);

  ::anyterm::screen::attribute_vector_t::const_iterator attribute_it;
  attribute_it = ::std::find_if(attributes_in.begin(), attributes_in.end(), ::boost::bind(::std::greater<
      ::anyterm::attribute >(), _1, attribute_out));

  if (attribute_it == attributes_in.begin()) {
    return ::anyterm::attribute();
  } else {
    return *(--attribute_it);
  }
}

struct row_transformer {
    row_transformer(::std::stringstream& stream_inout,
                    ::anyterm::screen::attribute_vector_t const& attributes_in,
                    ::std::uint32_t const row_number_in,
                    ::anyterm::attribute& last_attribute_in) :
      __stream(stream_inout), __attributes(attributes_in), __row_number(row_number_in), __column_number(0),
          __last_attribute(last_attribute_in) {
    }

    void operator()(char const& char_in) {
      ::anyterm::attribute attribute = get_attribute(__attributes, __row_number, __column_number);
      if (attribute != __last_attribute) {
        __stream << "</span>";
        __last_attribute = attribute;
        __stream << "<span class='" << __last_attribute.to_css() << "'>";
      }

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
          __stream << char_in;
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
    ::anyterm::screen::attribute_vector_t const& __attributes;
    ::std::uint32_t const __row_number;
    ::std::uint32_t __column_number;
    ::anyterm::attribute& __last_attribute;
};

struct lines_transformer {
    lines_transformer(::std::stringstream& stream_inout,
                      ::anyterm::screen::attribute_vector_t const& attributes_in) :
      __stream(stream_inout), __attributes(attributes_in), __row_number(0), __last_attribute() {
    }

    void operator()(::std::string const& row_in) {
      ::std::stringstream stream;
      ::std::for_each(row_in.begin(), row_in.end(), row_transformer(stream,
                                                                    __attributes,
                                                                    __row_number,
                                                                    __last_attribute));
      __stream << ::boost::trim_right_copy(stream.str()) << "<br/>";

      __row_number++;
    }

  private:
    ::std::stringstream& __stream;
    ::anyterm::screen::attribute_vector_t const& __attributes;
    ::std::uint32_t __row_number;
    ::anyterm::attribute __last_attribute;
};

::std::string htmlify_screen(::anyterm::screen const& screen_in) {
  ::anyterm::screen::line_vector_t const& lines = screen_in.lines();
  ::anyterm::screen::attribute_vector_t const& attributes = screen_in.attributes();

  ::std::stringstream html_stream;
  html_stream << "<span class='" << ::anyterm::attribute().to_css() << "'>";

  ::std::for_each(lines.begin(), lines.end(), lines_transformer(html_stream, attributes));

  //  for (::std::uint32_t i = 0; i < screen.row_count(); ++i) {
  //    ::anyterm::screen::const_row_view_t row_view = screen.row(i);
  //
  //    ::std::stringstream stream;
  //    ::std::for_each(row_view.begin(), row_view.end(), lines_transformer(html_stream, last_attributes));
  //    html_stream << ::boost::trim_right_copy(stream.str());
  //  }

  return ::boost::trim_right_copy_if(html_stream.str(), ::boost::is_any_of(" \n")) + "</span>";
}

