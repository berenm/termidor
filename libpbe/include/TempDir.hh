// include/TempDir.hh
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

#ifndef libpbe_TempDir_hh
#define libpbe_TempDir_hh


// Creates a temporary directory which is deleted, along with its contents,
// when the TempDir object goes out of scope.
// The directory is only accessible by the user that creates it.  mkdtemp()
// is used to do the work.
// The TempDir is convertible to a string giving the pathname of the
// directory.
// Optional constructor parameters specify the parent directory and an
// application-identifying string used in the directory name.

#include <Exception.hh>
#include <recursive_delete.hh>

#include <boost/scoped_array.hpp>

#include <stdlib.h>

#include <algorithm>
#include <string>


namespace pbe {

class TempDir {

  std::string pathname;

public:
  TempDir(std::string parent="/tmp", std::string app="tmp") {
    std::string templ = parent + "/" + app + ".XXXXXX";
    boost::scoped_array<char> buf(new char[templ.size()+1]);
    std::copy(templ.c_str(),templ.c_str()+templ.size()+1,buf.get());
    char* r = mkdtemp(buf.get());
    if (!r) {
      pbe::throw_ErrnoException("mkdtemp()");
    }
    pathname = buf.get();
  }

  ~TempDir() {
    recursive_delete(pathname);
  }

  operator std::string() const {
    return pathname;
  }

};

};


#endif

