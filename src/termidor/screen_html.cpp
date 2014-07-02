/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#include "termidor/screen.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

namespace termidor {

  std::string screen::to_html() const {
    typedef boost::u32_to_u8_iterator< std::wstring::iterator > to_utf8;

    screen::text_lines const& lines = this->lines;

    std::string        html;
    termidor::attribute last_attribute = termidor::attribute();
    std::uint32_t      row_number     = 0;
    std::uint32_t      column_number  = 0;

    html += "<span class='" + last_attribute.to_css() + "'>";

    auto const attrib_to_html = [&]() {
                                  termidor::attribute const& attribute = this->get_attribute(row_number, column_number);
                                  if (attribute == last_attribute)
                                    return;

                                  if (last_attribute != termidor::attribute())
                                    html += "</span>";

                                  if (attribute != termidor::attribute())
                                    html += "<span class='" + attribute.to_css() + "'>";

                                  last_attribute = attribute;
                                };

    auto const column_to_html = [&](wchar_t const& c) {
                                  attrib_to_html();

                                  std::wstring wstr(1, c);
                                  switch (c) {
                                    case '<':
                                      html += "&lt;";
                                      break;

                                    case '>':
                                      html += "&gt;";
                                      break;

                                    case '&':
                                      html += "&amp;";
                                      break;

                                    case '\n':
                                      html += "<br/>";
                                      break;

                                    default:
                                      html += std::string(to_utf8(wstr.begin()), to_utf8(wstr.end()));
                                      break;
                                  }

                                  column_number++;
                                };

    auto const row_to_html = [&](std::wstring const& row) {
                               column_number = 0;
                               std::for_each(row.begin(), row.end(), column_to_html);
                               boost::trim_right_if(html, boost::is_any_of(" \t"));
                               html += "<br/>";
                               row_number++;
                             };

    std::for_each(lines.begin(), lines.end(), row_to_html);

    boost::trim_right_if(html, boost::is_any_of(" \n"));
    html += "</span>";

    return html;
  } // to_html

}
