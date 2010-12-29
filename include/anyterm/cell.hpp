/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_CELL_HPP_
#define ANYTERM_CELL_HPP_

#include "anyterm/attributes.hpp"
#include "unicode.hh"

namespace anyterm {

  struct cell {
      ::std::uint32_t char_;
      attributes attributes_;

      cell(wchar_t const char_in = ' ');
      cell(wchar_t const char_in, attributes const& attributes_in);
  };

} // namespace anyterm

#endif /* ANYTERM_CELL_HPP_ */
