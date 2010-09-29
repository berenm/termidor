// include/reverse_pointer.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
// (C) 2009 Philip Endecott

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

#ifndef libpbe_reverse_pointer_hh
#define libpbe_reverse_pointer_hh


// reverse_pointer<T> is a T* that counts backwards.
// It is a convenience wrapper around std::reverse_iterator<T*>.


#include <iterator>


namespace pbe {


template <typename T>
struct reverse_pointer: std::reverse_iterator<T*> {
  reverse_pointer() {}
  reverse_pointer(const reverse_pointer& other):
    std::reverse_iterator<T*>(other) {}
  reverse_pointer(T* ptr):
    std::reverse_iterator<T*>(ptr) {}
};


};


#endif

