// include/contains.hh
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

#ifndef libpbe_contains_hh
#define libpbe_contains_hh


// Almost-trivial functions to test whether a string or some other container
// contains some other string or container.
// Also provides overloads for const char* and for single characters.


#include <algorithm>
#include <string>

#include <string.h>


namespace pbe {


template <typename CONTAINER>
inline bool contains(const CONTAINER& s, const CONTAINER& content) {
  return std::search(s.begin(),s.end(), content.begin(),content.end())
         != s.end();
}


inline bool contains(const std::string& s, const char* content) {
  size_t clen = strlen(content);
  return std::search(s.begin(),s.end(), content,content+clen)
         != s.end();
}

inline bool contains(const char* s, const char* content) {
  size_t slen = strlen(s);
  size_t clen = strlen(content);
  return std::search(s,s+slen, content,content+clen)
         != s+slen;
}

inline bool contains(const char* s, const std::string& content) {
  size_t slen = strlen(s);
  return std::search(s,s+slen, content.begin(),content.end())
         != s+slen;
}

inline bool contains(const std::string& s, const char content) {
  return std::find(s.begin(),s.end(), content)
         != s.end();
}



};


#endif

