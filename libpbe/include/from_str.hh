// include/from_str.hh
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

#ifndef libpbe_from_str_hh
#define libpbe_from_str_hh

#include <string>
#include <sstream>


namespace pbe {

template <typename T>
struct from_str_helper {
  T operator()(std::string s) { std::istringstream strm(s); T t; strm>>t; return t; }
};

template <>
struct from_str_helper<std::string> {
  std::string operator()(std::string s) { return s; }
};

template <>
struct from_str_helper<const char*> {
  std::string operator()(const char* s) { return s; }
};

template <typename T>
T from_str(std::string s) { return from_str_helper<T>()(s); }

template <typename T>
T from_str(const char* s) { return from_str_helper<T>()(s); }

};


#endif

