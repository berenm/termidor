/**
 * @file
 * @date Dec 29, 2010
 */

#ifndef ANYTERM_UNICODE_HPP_
#define ANYTERM_UNICODE_HPP_

#include <boost/regex/pending/unicode_iterator.hpp>

namespace anyterm {
  typedef ::boost::u32_to_u8_iterator< ::std::wstring::const_iterator > to_utf8;
  typedef ::boost::u8_to_u32_iterator< ::std::string::const_iterator > to_utf32;

} // namespace anyterm

#endif /* ANYTERM_UNICODE_HPP_ */
