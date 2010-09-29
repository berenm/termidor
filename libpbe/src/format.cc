// src/format.cc
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

#include "format.hh"

#include <string>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>


// vasprintf is a GNUism.

#ifdef __GLIBC__

namespace pbe {

std::string format(const char* fmt,...)
{
  va_list args;
  va_start(args,fmt);
  char* p;
  int r = vasprintf(&p,fmt,args);
  if (r<0) {
    throw "vasprintf failed";
  }
  va_end(args);
  std::string s(p,r);
  free(p);
  return s;
}

};

#else

#include <boost/scoped_array.hpp>

namespace pbe {

std::string format(const char* fmt,...)
{
  char p[128];
  va_list args;
  va_start(args,fmt);
  int r = vsnprintf(p,sizeof(p),fmt,args);
  // r is the number of characters needed for the result, excluding the terminating null.
  va_end(args);
  if (r<0) {
    throw "vsnprintf failed";
  }
  if (static_cast<unsigned int>(r)<sizeof(p)) {
    return std::string(p,r);
  }
  boost::scoped_array<char> q(new char[r+1]);
  va_start(args,fmt);
  r = vsnprintf(q.get(),r+1,fmt,args);
  va_end(args);
  if (r<0) {
    throw "vsnprintf failed";
  }
  return std::string(q.get(),r);
}

};

#endif


