// src/recursive_delete.hh
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

#include "recursive_delete.hh"

#include "Directory.hh"
#include "Exception.hh"
#include "FileType.hh"

#include <list>

using namespace std;

namespace pbe {

void recursive_delete(std::string path, bool ignore_errors)
{
  switch (get_filetype(path)) {
    case directory: {
      // We don't see . or .. thanks to Directory.
      // We must read the directory before changing its contents,
      // else bad things will happen.
      typedef list<string> subpaths_t;
      subpaths_t subpaths;
      {
        Directory d(path);
        for (Directory::const_iterator i = d.begin();
             i != d.end(); ++i) {
          subpaths.push_back(i->pathname);
        }
      }
      for (subpaths_t::const_iterator i = subpaths.begin();
           i != subpaths.end(); ++i) {
        recursive_delete(*i,ignore_errors);
      }
      int r = rmdir(path.c_str());
      if (r==-1 && !ignore_errors) {
        throw_ErrnoException("rmdir("+path+")");
      }
      break;
    }
    default: {
      // Regular files, symlinks, specials.
      int r = unlink(path.c_str());
      if (r==-1 && !ignore_errors) {
        throw_ErrnoException("unlink("+path+")");
      }
      break;
    }
  }
}


};

