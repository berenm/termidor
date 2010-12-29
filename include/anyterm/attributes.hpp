/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_ATTRIBUTES_HPP_
#define ANYTERM_ATTRIBUTES_HPP_

#include <cstdint>

namespace anyterm {

  struct attributes {
      ::std::uint8_t foreground_;
      ::std::uint8_t background_;
      bool halfbright_;
      bool bold_;
      bool underlined_;
      bool blink_;
      bool inverse_;

      attributes();
      bool operator==(attributes const& other) const;
      bool operator!=(attributes const& other) const;
  };

} // namespace anyterm

#endif /* ANYTERM_ATTRIBUTES_HPP_ */
