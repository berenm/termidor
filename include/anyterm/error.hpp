/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#ifndef __ANYTERM_ERROR_HPP__
#define __ANYTERM_ERROR_HPP__

#include <boost/exception/all.hpp>

namespace anyterm {

  struct error : virtual std::exception, virtual boost::exception {};

} // namespace anyterm

#endif // ifndef __ANYTERM_ERROR_HPP__
