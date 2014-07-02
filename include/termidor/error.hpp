/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#ifndef __TERMIDOR_ERROR_HPP__
#define __TERMIDOR_ERROR_HPP__

#include <boost/exception/all.hpp>

namespace termidor {

  struct error : virtual std::exception, virtual boost::exception {};

} // namespace termidor

#endif // ifndef __TERMIDOR_ERROR_HPP__
