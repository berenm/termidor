// include/ordered_find.hh
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

#ifndef libpbe_ordered_find_hh
#define libpbe_ordered_find_hh

#include <algorithm>


namespace pbe {

// Like std::find, except that it assumes the input range is ordered and 
// so operates in logarithmic time.
// Also like std::lower_bound and std::equal_range, except that it returns 
// end if the item is not found, and returns just one item if more than 
// one matches.
// (Implemented using std::lower_bound.)


template <class ITER, class VAL_T>
ITER ordered_find(ITER first, ITER last, const VAL_T& value)
{
  ITER i = std::lower_bound(first,last,value);
  if (i == last || value < *i) {
    return last;
  } else {
    return i;
  }
}


template <class ITER, class VAL_T, class LESS_PRED>
ITER ordered_find(ITER first, ITER last, const VAL_T& value, LESS_PRED less_pred)
{
  ITER i = std::lower_bound(first,last,value,less_pred);
  if (i == last || less_pred(value,*i)) {
    return last;
  } else {
    return i;
  }
}
  

};


#endif




