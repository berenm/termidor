// include/format_bytes.hh
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

#ifndef libpbe_format_bytes_hh
#define libpbe_format_bytes_hh

#include "format.hh"

namespace pbe {


inline std::string format_bytes(size_t bytes) {
  if (bytes<1024) {
    return format("%lu B",bytes);
  } else if (bytes<(10*1024)) {
    return format("%.1f kB",bytes/1024.0);
  } else if (bytes<(1024*1024)) {
    return format("%lu kB",bytes/1024);
  } else if (bytes<(10*1024*1024)) {
    return format("%.1f MB",bytes/(1024.0*1024));
  } else if (bytes<(1024*1024*1024)) {
    return format("%lu MB",bytes/(1024*1024));
  } else {
    return format("%.1f GB",bytes/(1024.0*1024*1024));
  }
}


};


#endif

