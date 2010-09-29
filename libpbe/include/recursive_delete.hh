// include/recursive_delete.hh
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


// Given a filesystem path, recursvely delete it.
// Symbolic links are deleted, but the things that they point to aren't.


#ifndef libpbe_recursive_delete_hh
#define libpbe_recursive_delete_hh

#include <string>


namespace pbe {

extern void recursive_delete(std::string path, bool ignore_errors = false);

inline void try_recursive_delete(std::string path) { recursive_delete(path,true); }

};

#endif
