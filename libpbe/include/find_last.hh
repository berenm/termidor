// include/find_last.hh
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

#ifndef libpbe_find_last_hh
#define libpbe_find_last_hh

namespace pbe {

// Like std::find and std::find_if, except that it finds the last matching 
// element not the first.  If the element is not found, it returns end (careful!).
// This implementation relies on ITER being a bidirectional iterator. 
// To work with forward iterators it is necessary to scan forwards, but 
// that is not optimal when the iterator is bidirectional.  A specialisation 
// based on iterator category would fix that.


template <typename ITER, typename VAL_T>
ITER find_last(ITER begin, ITER end, VAL_T value)
{
  for (ITER i = end; i != begin;) {
    --i;
    if (*i == value) {
      return i;
    }
  }
  return end;
}


template <typename ITER, typename PRED_T>
ITER find_last_if(ITER begin, ITER end, PRED_T pred)
{
  for (ITER i = end; i != begin;) {
    --i;
    if (pred(*i)) {
      return i;
    }
  }
  return end;
}


};


#endif




