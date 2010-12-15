/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ATTRIBUTES_HPP_
#define ATTRIBUTES_HPP_

#include <boost/cstdint.hpp>

struct attributes {
    ::std::uint8_t foreground;
    ::std::uint8_t background;
    bool halfbright;
    bool bold;
    bool underlined;
    bool blink;
    bool inverse;

    attributes() :
      foreground(7), background(0), halfbright(false), bold(false), underlined(false), blink(false),
          inverse(false) {
    }

    bool operator==(attributes const& other) const {
      return foreground == other.foreground && background == other.background && halfbright
          == other.halfbright && bold == other.bold && underlined == other.underlined && blink == other.blink
          && inverse == other.inverse;
    }

    bool operator!=(attributes const& other) const {
      return !operator==(other);
    }
};

#endif /* ATTRIBUTES_HPP_ */
