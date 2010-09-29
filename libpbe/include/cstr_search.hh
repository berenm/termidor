// include/cstr_search.hh
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

#ifndef libpbe_cstr_search_hh
#define libpbe_cstr_search_hh


// Like std::search but takes a const char* for the pattern.

// This starts by finding the length of the pattern.  That's not optimal if
// the pattern is long and will never be found; it would be better in that case 
// to have a smart end pointer that knows when it's pointing at a terminating 
// null.
// FIXME we now have const_cstr_iterator to do this.


#include <algorithm>
#include <string>  // for char_traits<char>


namespace pbe {

template <typename ITER>
ITER search(ITER begin, ITER end, const char* pat) {
  return std::search(begin,end, pat,pat+std::char_traits<char>::length(pat));
}

};

#endif
