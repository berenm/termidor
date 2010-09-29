// include/elemsof.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
// (C) 2008 Philip Endecott

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

#ifndef libpbe_elemsof_hh
#define libpbe_elemsof_hh

namespace pbe {


// Return the number of elements in an array.  Usage:
// sometype X[231];
// assert(elemsof(X)==231);

// There is also an overload in init_array.hh.  Overloads for e.g. boost::array
// could be added here.


template <typename T, size_t N>
inline size_t elemsof(const T (&) [N]) {
  return N;
}


};


#endif

