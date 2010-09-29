// include/pathnames.hh
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

#ifndef libpbe_pathnames_hh
#define libpbe_pathnames_hh

#include "find_last.hh"
#include "succpred.hh"

#include <string>


namespace pbe {

typedef std::string pathname_t;
// A pathname must meet the following invariants:
// - Always absolute, i.e. starts with a /.
// - Contains no // anywhere.
// - Contains no . or .. components.
// - Does not end with a trailing / even if it the path to a directory.
// Note: the first and last points are contradictory in the case of the root
// directory.  It is therefore NOT POSSIBLE TO REPRESENT THE ROOT DIRECTORY 
// AS A pathname_t, i.e. the following functions have undefined behaviour if 
// they are passed a pathname_t of "/" or "".


// Returns the last component of a pathname:
inline std::string leafname(pathname_t p) {
  return std::string(succ(find_last(p.begin(),p.end(),'/')),p.end());
}


// Returns the absolute path (as above i.e. with no trailing /) of the directory containing p:
inline pathname_t dirname(pathname_t p) {
  return std::string(p.begin(),find_last(p.begin(),p.end(),'/'));
}


// Returns the extension.  This includes the '.', and if there is no extension, is "".
inline std::string extension(std::string fn) {
  return std::string(find_last(fn.begin(),fn.end(),'.'),fn.end());
}


};


#endif
