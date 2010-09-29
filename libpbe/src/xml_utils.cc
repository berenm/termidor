// src/xml_utils.cc
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
// (C) 2009-2010 Philip Endecott

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

#include "xml_utils.hh"

#include <predicates.hh>
#include <succpred.hh>

#include <string>
#include <algorithm>


namespace pbe {

extern std::string escape_quotes_for_xml(const std::string s)
{
  std::string::const_iterator j = std::find_if(s.begin(), s.end(), &is_any_of<'"','&'>);
  if (j==s.end()) {
    return s;
  }
  std::string::const_iterator i = s.begin();
  std::string t;
  while (1) {
    t.append(i,j);
    if (j==s.end()) {
      break;
    }
    switch (*j) {
      case '"': t+="&quot;"; break;
      case '&': t+="&amp;"; break;
    }
    i = succ(j);
    j = std::find_if(i, s.end(), &is_any_of<'"','&'>);
  }
  return t;
}


extern std::string escape_for_xml(const std::string s)
{
  std::string::const_iterator j = std::find_if(s.begin(), s.end(), &is_any_of<'<','>','&'>);
  if (j==s.end()) {
    return s;
  }
  std::string::const_iterator i = s.begin();
  std::string t;
  while (1) {
    t.append(i,j);
    if (j==s.end()) {
      break;
    }
    switch (*j) {
      case '<': t+="&lt;"; break;
      case '>': t+="&gt;"; break;
      case '&': t+="&amp;"; break;
    }
    i = succ(j);
    j = std::find_if(i, s.end(), &is_any_of<'<','>','&'>);
  }
  return t;
}


};

