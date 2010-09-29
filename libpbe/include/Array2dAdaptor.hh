// Array2dAdaptor.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe
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

#ifndef libpbe_Array2dAdaptor_hh
#define libpbe_Array2dAdaptor_hh

// Adapts a random access container into a 2D array.
// (Well, it takes a begin iterator, not the container.)
// Dimensions are template parameters.

#include <cstddef>
#include <iterator>


namespace pbe {


template <typename IMPL_T, size_t w, size_t h>
class Array2dAdaptor {

  IMPL_T& impl;

public:
  Array2dAdaptor(IMPL_T& impl_): impl(impl_) {}
  typename std::iterator_traits<IMPL_T>::reference operator()(size_t x, size_t y) {
    size_t i = y*w + x;
    return impl[i];
  }

};


};


#endif
