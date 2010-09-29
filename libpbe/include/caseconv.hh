// include/caseconv.hh
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

#ifndef libpbe_caseconv_hh
#define libpbe_caseconv_hh

#include <string>
#include <cctype>


namespace pbe {


inline std::string toupper(std::string s)
{
  std::string t;
  t.reserve(s.length());
  for (std::string::const_iterator i = s.begin();
       i != s.end(); ++i) {
    t += std::toupper(*i);
  }
  return t;
}


inline std::string tolower(std::string s)
{
  std::string t;
  t.reserve(s.length());
  for (std::string::const_iterator i = s.begin();
       i != s.end(); ++i) {
    t += std::tolower(*i);
  }
  return t;
}


inline bool insens_less(std::string a, std::string b)
{
  std::string::const_iterator i = a.begin();
  std::string::const_iterator j = b.begin();
  while (i != a.end() && j != b.end()) {
    char c = std::tolower(*i);
    char d = std::tolower(*j);
    if (c<d) {
      return true;
    } else if (d<c) {
      return false;
    }
    ++i;
    ++j;
  }
  return i==a.end() && j!=b.end();
}


};


#endif

