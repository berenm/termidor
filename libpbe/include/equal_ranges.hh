// include/equal_ranges.hh
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

#ifndef libpbe_equal_ranges_hh
#define libpbe_equal_ranges_hh

namespace pbe {

// std::equal takes 3 parameters, the start and end of range 1 and the start 
// of range 2.  The caller is responsible for coping with the cases where range 2 
// has a different length than range 1.  Although this is doable for a random-
// access range, it's not efficient to pre-compute the lengths of forward 
// ranges when the difference might be found in the first element.
//
// equal_ranges takes 4 parameters, i.e. the start and end of both ranges, 
// making efficient comparison possible in this case.


template <class InputIterator1, class InputIterator2>
bool equal_ranges(InputIterator1 first1, InputIterator1 last1,
                  InputIterator2 first2, InputIterator2 last2)
{
  while (1) {
    bool at_end1 = first1==last1;
    bool at_end2 = first2==last2;
    if (at_end1 && at_end2) return true;
    if (at_end1 || at_end2) return false;
    if (*first1 != *first2) return false;
    ++first1;
    ++first2;
  }
}




};


#endif




