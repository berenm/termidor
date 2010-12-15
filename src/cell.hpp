/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef CELL_HPP_
#define CELL_HPP_

#include "attributes.hpp"
#include "unicode.hh"

struct cell {
    ucs4_char c;
    attributes attributes_;

    cell(ucs4_char const char_in = ' ') :
      c(char_in) {
    }

    cell(ucs4_char const char_in, attributes const attributes_in) :
      c(char_in), attributes_(attributes_in) {
    }
};

#endif /* CELL_HPP_ */
