// include/succpred.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
// (C) 2010 Philip Endecott

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef libpbe_succpred_hh
#define libpbe_succpred_hh

#include <boost/type_traits/is_enum.hpp>

namespace pbe {


// Return the successor and predecessor of a value respectively.
// Works in the obvious way for types that provide ++ and -- 
// (i.e. integers, pointers, iterators etc.)
// Enum's don't, but we can cast to int (assuming that the enums 
// are contiguous).  Other types that need more complex support 
// could also be added.


template <typename T>
T do_succ(T val, boost::false_type) {
  ++val;
  return val;
}

template <typename E>
E do_succ(E val, boost::true_type) {
  return static_cast<E>( static_cast<int>(val) + 1 );
}

template <typename T>
T succ(T val) {
  return do_succ<T>(val, typename boost::is_enum<T>::type());
}


template <typename T>
T do_pred(T val, boost::false_type) {
  --val;
  return val;
}

template <typename E>
E do_pred(E val, boost::true_type) {
  return static_cast<E>( static_cast<int>(val) - 1 );
}

template <typename T>
T pred(T val) {
  return do_pred<T>(val, typename boost::is_enum<T>::type());
}


};


#endif




