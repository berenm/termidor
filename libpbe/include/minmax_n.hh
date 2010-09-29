// include/minmax_n.hh
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

#ifndef libpbe_minmax_n_hh
#define libpbe_minmax_n_hh


// min() and max() functions that take 3, 4 and 5 arguments.


#include <algorithm>

namespace pbe {

template <typename T>
T min(T a, T b, T c) {
  return std::min<T>(std::min<T>(a,b),c);
}

template <typename T>
T min(T a, T b, T c, T d) {
  return std::min<T>(pbe::min<T>(a,b,c),d);
}

template <typename T>
T min(T a, T b, T c, T d, T e) {
  return std::min<T>(pbe::min<T>(a,b,c,d),e);
}

template <typename T>
T min(T a, T b, T c, T d, T e, T f) {
  return std::min<T>(pbe::min<T>(a,b,c,d,e),f);
}

template <typename T>
T max(T a, T b, T c) {
  return std::max<T>(std::max<T>(a,b),c);
}

template <typename T>
T max(T a, T b, T c, T d) {
  return std::max<T>(pbe::max<T>(a,b,c),d);
}

template <typename T>
T max(T a, T b, T c, T d, T e) {
  return std::max<T>(pbe::max<T>(a,b,c,d),e);
}

template <typename T>
T max(T a, T b, T c, T d, T e, T f) {
  return std::max<T>(pbe::max<T>(a,b,c,d,e),f);
}


};


#endif



