// include/disk_space.hh
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


// Given a filesystem path, return the number of bytes available on
// the filesystem that contains it.  Return type is uint64_t.

#ifndef libpbe_disk_space_hh
#define libpbe_disk_space_hh

#include "Exception.hh"

// statvfs() is one of the more portable of the various *stat*fs*() calls.
#include <sys/statvfs.h>

#include <stdint.h>

#include <string>


namespace pbe {

inline uint64_t disk_space(std::string path) {

  struct ::statvfs s;
  int r = ::statvfs(path.c_str(), &s);
  if (r==-1) {
    throw_ErrnoException("statvfs()");
  }
  return static_cast<uint64_t>(s.f_frsize) * static_cast<uint64_t>(s.f_bavail);
}

};

#endif
