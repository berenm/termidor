// include/sort2.hh
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

#ifndef libpbe_sort2_hh
#define libpbe_sort2_hh


// Sort using two predicates, the second being used when the first indicates that 
// the elements are equal.

// (This implementation uses a two-pass method.  A single-pass with a merged predicate 
// may or may not be faster.)


#include <algorithm>


namespace pbe {


template <typename ITER, typename PRED1, typename PRED2>
void sort2(ITER begin, ITER end, PRED1 pred1, PRED2 pred2) {
  std::sort(begin,end,pred2);
  std::stable_sort(begin,end,pred1);
}


};

#endif

