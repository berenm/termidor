/**
 * @file
 * @date Dec 29, 2010
 */

#ifndef ANYTERM_ERROR_HPP_
#define ANYTERM_ERROR_HPP_

#include <boost/exception/all.hpp>

namespace anyterm {

  struct error: virtual ::std::exception, virtual ::boost::exception {
  };

} // namespace anyterm

#endif /* ANYTERM_ERROR_HPP_ */
