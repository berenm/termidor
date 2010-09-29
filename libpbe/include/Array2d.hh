// Array2d.hh
// This file is part of libpbe; see http://anyterm.org/
// (C) 2006 Philip Endecott

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

#ifndef libpbe_Array2d_hh
#define libpbe_Array2d_hh

#include <boost/array.hpp>


namespace pbe {

template <typename T, std::size_t N, std::size_t M >
class Array2d: boost::array<T,N*M> {

public:
  Array2d() {}
  Array2d(const T& t) {
    for (size_t i=0; i<N*M; ++i) {
      (*this)[i] = t;
    }
  }

  const T& operator()(std::size_t x, std::size_t y) const {
    return (*this)[y*N + x];
  }

  T& operator()(std::size_t x, std::size_t y) {
    return (*this)[y*N + x];
  }

  unsigned int width() const { return N; }
  unsigned int height() const { return M; }
};

};


#endif
