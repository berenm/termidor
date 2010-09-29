// include/to_str.hh
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

#ifndef libpbe_to_str_hh
#define libpbe_to_str_hh

#include <format.hh>

#include <string>
#include <sstream>


namespace pbe {

template <typename T>
struct to_str_helper {
  std::string operator()(T t) { std::ostringstream s; s<<t; return s.str(); }
};

template <>
struct to_str_helper<std::string> {
  std::string operator()(std::string s) { return s; }
};

template <>
struct to_str_helper<const char*> {
  std::string operator()(const char* s) { return s; }
};

template <>
struct to_str_helper<int> {
  std::string operator()(int i) { return format("%d",i); }
};

template <>
struct to_str_helper<unsigned int> {
  std::string operator()(unsigned int i) { return format("%u",i); }
};

template <>
struct to_str_helper<float> {
  std::string operator()(float f) { return format("%f",f); }  // %f means double; the float is promoted.
};

template <>
struct to_str_helper<double> {
  std::string operator()(double d) { return format("%lf",d); }  // %lf also means double.
};


template <typename T>
std::string to_str(T t) { return to_str_helper<T>()(t); }

};


#endif

