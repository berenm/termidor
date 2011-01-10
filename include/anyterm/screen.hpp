/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_SCREEN_HPP_
#define ANYTERM_SCREEN_HPP_

#include "anyterm/attribute.hpp"

#include <boost/multi_array.hpp>

#include <vector>
#include <algorithm>

namespace anyterm {

  struct screen {
      typedef ::std::vector< ::std::wstring > line_vector_t;
      typedef ::std::vector< attribute > attribute_line_t;
      typedef ::std::vector< attribute_line_t > attribute_lines_t;

      screen(line_vector_t const& lines_in, attribute_lines_t attributes_in = attribute_lines_t());
      ~screen();

      line_vector_t const& lines() const;
      attribute_lines_t const& attributes() const;

      ::anyterm::attribute get_attribute(::std::uint32_t const row_number_in,
                                         ::std::uint32_t const column_number_in) const;
      void set_attribute(::std::uint32_t const row_number_in,
                         ::std::uint32_t const column_number_in,
                         ::anyterm::attribute const& attribute_in);

    private:
      line_vector_t __lines;
      attribute_lines_t __attributes;
  };

} // namespace anyterm

#endif /* ANYTERM_SCREEN_HPP_ */
