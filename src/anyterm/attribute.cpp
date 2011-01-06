/**
 * @file
 * @date Dec 16, 2010
 */

#include "anyterm/attribute.hpp"

#include <boost/lexical_cast.hpp>

namespace anyterm {

  attribute::attribute() :
    __foreground("def_fg"), __background("def_bg"), __underlined(false), __strikethrough(false)
  //  , __halfbright(false), __bold(false), __blink(false), __inverse(false)
  {
  }

  bool attribute::operator==(attribute const& other_in) const {
    return __foreground.compare(other_in.__foreground) == 0 && __background.compare(other_in.__background)
        == 0 && __underlined == other_in.__underlined && __strikethrough == other_in.__strikethrough
    //        && __halfbright == other_in.__halfbright && __bold == other_in.__bold && __blink == other_in.__blink && __inverse == other_in.__inverse
    ;
  }

  bool attribute::operator!=(attribute const& other_in) const {
    return !operator==(other_in);
  }

  bool attribute::operator<(attribute const& other_in) const {
    return __row < other_in.__row || (__row == other_in.__row && __column < other_in.__column);
  }
  bool attribute::operator<=(attribute const& other_in) const {
    return __row < other_in.__row || (__row == other_in.__row && __column <= other_in.__column);
  }
  bool attribute::operator>(attribute const& other_in) const {
    return __row > other_in.__row || (__row == other_in.__row && __column > other_in.__column);
  }
  bool attribute::operator>=(attribute const& other_in) const {
    return __row > other_in.__row || (__row == other_in.__row && __column >= other_in.__column);
  }

  void attribute::set_row(::std::uint32_t const row_in) {
    __row = row_in;
  }

  void attribute::set_column(::std::uint32_t const column_in) {
    __column = column_in;
  }

  void attribute::set_foreground(::std::string const& foreground_in) {
    __foreground = foreground_in;
  }
  void attribute::set_background(::std::string const& background_in) {
    __background = background_in;
  }
  void attribute::set_underlined(bool const underlined_in) {
    __underlined = underlined_in;
  }
  void attribute::set_strikethrough(bool const strikethrough_in) {
    __strikethrough = strikethrough_in;
  }
  //  void attribute::set_halfbright(bool const halfbright_in) {
  //    __halfbright = halfbright_in;
  //  }
  //  void attribute::set_bold(bool const bold_in) {
  //    __bold = bold_in;
  //  }
  //  void attribute::set_blink(bool const blink_in) {
  //    __blink = blink_in;
  //  }
  //  void attribute::set_inverse(bool const inverse_in) {
  //    __inverse = inverse_in;
  //  }
  //  void attribute::set_cursor(bool const cursor_in) {
  //    __cursor = cursor_in;
  //  }

  ::std::uint32_t attribute::row() {
    return __row;
  }

  ::std::uint32_t attribute::column() {
    return __column;
  }

  ::std::string attribute::to_css() const {
    ::std::string css_classes = "attributes ";
    css_classes += "foreground_" + __foreground + " ";
    css_classes += "background_" + __background + " ";
    css_classes += ::std::string("underlined_") + (__underlined ? "yes" : "no") + " ";
    css_classes += ::std::string("strikethrough_") + (__strikethrough ? "yes" : "no") + " ";
    //    css_classes += ::std::string("halfbright_") + (__halfbright ? "yes" : "no");
    //    css_classes += ::std::string("bold_") + (__bold ? "yes" : "no");
    //    css_classes += ::std::string("blink_") + (__blink ? "yes" : "no");
    //    css_classes += ::std::string("inverse_") + (__inverse ? "yes" : "no");
    //    css_classes += ::std::string("cursor_") + (__cursor ? "yes" : "no");

    return css_classes;
  }

} // namespace anyterm
