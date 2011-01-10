/**
 * @file
 * @date Dec 16, 2010
 */

#include "anyterm/screen.hpp"

#include <iostream>

#include <boost/bind.hpp>

namespace anyterm {

  screen::screen(line_vector_t const& lines_in, attribute_lines_t attributes_in) :
    __lines(lines_in), __attributes(attributes_in) {
    ::std::sort(__attributes.begin(), __attributes.end());
  }
  screen::~screen() {
  }

  screen::line_vector_t const& screen::lines() const {
    return __lines;
  }
  screen::attribute_lines_t const& screen::attributes() const {
    return __attributes;
  }

  ::anyterm::attribute screen::get_attribute(::std::uint32_t const row_number_in,
                                             ::std::uint32_t const column_number_in) const {
    //    ::anyterm::attribute attribute_out;
    //    attribute_out.set_row(row_number_in);
    //    attribute_out.set_column(column_number_in);
    //
    if (__attributes.size() < row_number_in + 1) {
      return attribute();
    }

    attribute_line_t const& attributes = __attributes[row_number_in];
    if (attributes.size() < column_number_in + 1) {
      return attribute();
    } else {
      ::anyterm::attribute attribute_out = attributes[column_number_in];
      attribute_out.set_row(row_number_in);
      attribute_out.set_column(column_number_in);
      return attribute_out;
    }
    //
    //    ::anyterm::screen::attribute_vector_t::const_iterator attribute_it;
    //    attribute_it = ::std::find_if(__attributes.begin(), __attributes.end(), ::boost::bind(::std::greater<
    //        ::anyterm::attribute >(), _1, attribute_out));
    //
    //    if (attribute_it == __attributes.begin()) {
    //      return ::anyterm::attribute();
    //    } else {
    //      return *(--attribute_it);
    //    }
  }

  void screen::set_attribute(::std::uint32_t const row_number_in,
                             ::std::uint32_t const column_number_in,
                             ::anyterm::attribute const& attribute_in) {
    ::anyterm::attribute attribute_out = attribute_in;
    attribute_out.set_row(row_number_in);
    attribute_out.set_column(column_number_in);

    if (__attributes.size() < row_number_in + 1) {
      __attributes.resize(row_number_in + 1, attribute_line_t());
    }

    attribute_line_t& attributes = __attributes[row_number_in];
    if (attributes.size() < column_number_in + 1) {
      attributes.resize(column_number_in + 1, attribute());
    }

    attributes[column_number_in] = attribute_out;

    //    ::anyterm::screen::attribute_vector_t::iterator attribute_it;
    //    attribute_it
    //        = ::std::find_if(__attributes.begin(), __attributes.end(), ::boost::bind(::std::greater_equal<
    //            ::anyterm::attribute >(), _1, attribute));
    //
    //    if (attribute_it != __attributes.end()) {
    //      if (attribute_it->row() == attribute.row() && attribute_it->column() == attribute.column()) {
    //        __attributes.erase(attribute_it);
    //      }
    //    }
    //
    //    __attributes.push_back(attribute);
    //    ::std::sort(__attributes.begin(), __attributes.end());

    if (__lines.size() < row_number_in + 1) {
      __lines.resize(row_number_in + 1, ::std::wstring(column_number_in, L' '));
    }

    ::std::wstring& line = __lines[row_number_in];
    if (line.size() < column_number_in) {
      line.append(::std::wstring(column_number_in - line.size(), L' '));
    }
  }

} // namespace anyterm
