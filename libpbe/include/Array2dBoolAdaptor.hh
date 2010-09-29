// Array2dBoolAdaptor.hh
// This file is part of libpbe; see http://anyterm.org/
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

#ifndef libpbe_Array2dBoolAdaptor_hh
#define libpbe_Array2dBoolAdaptor_hh

// Adapts a random access container of bytes into a 2D array of bools.
// Can be used with e.g. a vector<uint8_t> or raw pointers.
// Dimensions are template parameters.

#include <stdint.h>
#include <cstddef>


namespace pbe {


template <size_t w, size_t h, typename data_t>
class Array2dBoolAdaptor {

  data_t data;

  struct assign_proxy {
    uint8_t& data;
    int bitnum;
    assign_proxy(uint8_t& data_, int bitnum_): data(data_), bitnum(bitnum_) {}
    void operator=(bool b) {
      if (b) data = data | (1<<bitnum);
      else   data = data & ~(1<<bitnum);
    }
    operator bool() const {
      return data & (1<<bitnum);
    }
  };

public:
  Array2dBoolAdaptor(data_t data_): data(data_) {}
  assign_proxy operator()(size_t x, size_t y) {
    size_t i = y*w + x;
    return assign_proxy(data[i/8],i%8);
  }

};


};


#endif
