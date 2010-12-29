/**
 * @file
 * @date Dec 16, 2010
 */

#include "anyterm/cell.hpp"

namespace anyterm {

  cell::cell(ucs4_char const char_in) :
    char_(char_in) {
  }

  cell::cell(ucs4_char const char_in, attributes const& attributes_in) :
    char_(char_in), attributes_(attributes_in) {
  }

} // namespace anyterm
