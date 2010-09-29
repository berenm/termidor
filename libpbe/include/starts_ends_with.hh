// include/starts_ends_with.hh
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

#ifndef libpbe_starts_ends_with_hh
#define libpbe_starts_ends_with_hh


// Almost-trivial functions to test whether a string or some other container
// starts or ends with some other string or container.
// Additional overloads are provided for testing a std::string and a 
// const char* or a char


#include <algorithm>
#include <string>

#include <string.h>  // for strlen()


namespace pbe {


template <typename CONTAINER>
inline bool starts_with(const CONTAINER& s, const CONTAINER& prefix) {
  return (s.size() >= prefix.size())
      && std::equal(prefix.begin(),prefix.end(), s.begin());
}


template <typename CONTAINER>
inline bool ends_with(const CONTAINER& s, const CONTAINER& suffix) {
  return (s.size() >= suffix.size())
      && std::equal(suffix.rbegin(),suffix.rend(), s.rbegin());
}


inline bool starts_with(const std::string& s, const char* prefix) {
  size_t len = strlen(prefix);
  return (s.size() >= len)
      && std::equal(prefix,prefix+len, s.begin());
}


inline bool ends_with(const std::string& s, const char* suffix) {
  size_t len = strlen(suffix);
  return (s.size() >= len)
      && std::equal(suffix,suffix+len, s.end()-len);
}


inline bool starts_with(const std::string& s, char prefix) {
  return (s.size() >= 1) && (*(s.begin()) == prefix);
}


inline bool ends_with(const std::string& s, char suffix) {
  return (s.size() >= 1) && (*(s.end()-1) == suffix);
}


};


#endif

