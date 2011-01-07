/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_ATTRIBUTE_HPP_
#define ANYTERM_ATTRIBUTE_HPP_

#include <cstdint>
#include <string>

namespace anyterm {

  struct attribute {
      attribute();

      bool operator==(attribute const& other_in) const;
      bool operator!=(attribute const& other_in) const;
      bool operator<(attribute const& other_in) const;
      bool operator<=(attribute const& other_in) const;
      bool operator>(attribute const& other_in) const;
      bool operator>=(attribute const& other_in) const;

      void set_row(::std::uint32_t const row_in);
      void set_column(::std::uint32_t const column_in);
      void set_foreground(::std::string const& foreground_in);
      void set_background(::std::string const& foreground_in);
      void set_underlined(bool const underlined_in);
      void set_strikethrough(bool const strikethrough_in);
      void set_cursor(bool const cursor_in);
      //      void set_halfbright(bool const halfbright_in);
      //      void set_bold(bool const bold_in);
      //      void set_blink(bool const blink_in);
      //      void set_inverse(bool const inverse_in);
      //      void set_cursor(bool const cursor_in);

      ::std::uint32_t row() const;
      ::std::uint32_t column() const;

      ::std::string to_css() const;

    protected:
      ::std::uint32_t __row;
      ::std::uint32_t __column;
      ::std::string __foreground;
      ::std::string __background;
      bool __underlined;
      bool __strikethrough;
      bool __cursor;
      //      bool __halfbright;
      //      bool __bold;
      //      bool __blink;
      //      bool __inverse;
      //      bool __cursor;
  };

} // namespace anyterm

#endif /* ANYTERM_ATTRIBUTE_HPP_ */
