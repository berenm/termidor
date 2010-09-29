// RtArray2d.hh
// This file is part of libpbe; see http://anyterm.org/
// (C) 2009 Philip Endecott
//
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

#ifndef libpbe_RtArray2d_hh
#define libpbe_RtArray2d_hh

#include <vector>
#include <boost/dynamic_bitset.hpp>

// "Run time" 2d array, i.e. the dimensions are specified as ctor parameters.
// Optimised 1-bit-per-element implementation for RtArray2d<bool>.


namespace pbe {


template <typename T>
struct vector_helper {
  typedef typename std::vector<T> type;
};

template <>
struct vector_helper<bool> {
  typedef boost::dynamic_bitset<> type;
};


template <typename T, typename Impl = typename vector_helper<T>::type >
class RtArray2d {

public:
  RtArray2d():
    width_(0), height_(0)
  {}

  RtArray2d(unsigned int w, unsigned int h):
    data(w*h), width_(w), height_(h)
  {}

  RtArray2d(unsigned int w, unsigned int h, T value):
    data(w*h,value), width_(w), height_(h)
  {}

  typename Impl::const_reference operator()(unsigned int x, unsigned int y) const {
    return data[y*width_ + x];
  }

  typename Impl::reference operator()(unsigned int x, unsigned int y) {
    return data[y*width_ + x];
  }

  unsigned int width() const { return width_; }
  unsigned int height() const { return height_; }

private:
  Impl data;
  unsigned int width_;
  unsigned int height_;
};

};


#endif
