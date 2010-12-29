/**
 * @file
 * @date Dec 16, 2010
 */

#include "anyterm/attributes.hpp"

namespace anyterm {

  attributes::attributes() :
    foreground_(7), background_(0), halfbright_(false), bold_(false), underlined_(false), blink_(false),
        inverse_(false) {
  }

  bool attributes::operator==(attributes const& other) const {
    return foreground_ == other.foreground_ && background_ == other.background_ && halfbright_
        == other.halfbright_ && bold_ == other.bold_ && underlined_ == other.underlined_ && blink_
        == other.blink_ && inverse_ == other.inverse_;
  }

  bool attributes::operator!=(attributes const& other) const {
    return !operator==(other);
  }

} // namespace anyterm
