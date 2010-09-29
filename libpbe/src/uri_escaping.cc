// src/uri_escaping.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe
// (C) 2010 Philip Endecott

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

#include "uri_escaping.hh"

#include "predicates.hh"
#include "strto.hh"
#include "format.hh"

#include <boost/integer.hpp>
#include <algorithm>

using namespace std;

namespace pbe {

string uri_unescape(const string esc)
{
  string unesc;
  string::const_iterator p = esc.begin();
  while (p != esc.end()) {
    string::const_iterator q = find_if(p,esc.end(),&is_any_of<'%','+'>);
    unesc.append(p,q);
    if (q==esc.end()) {
      break;
    }
    if (*q == '+') {
      unesc += ' ';
      p = q+1;
    } else {
      if (q+2 >= esc.end()) {
        // malformed; not enough characters after %.
        throw MalformedUriEscaping();
      }
      unesc += static_cast<char>(hex_strtol(q+1,q+3));
      p = q+3;   
    }
  }
  return unesc;
}


static bool is_not_uri_unreserved_char(char c)
{
  return ! ( (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9')
             || is_any_of<'-','_','.','~'>(c) );
}
  

string uri_escape(const string unesc)
{
  string esc;
  string::const_iterator p = unesc.begin();
  while (p != unesc.end()) {
    string::const_iterator q = find_if(p,unesc.end(),&is_not_uri_unreserved_char);
    esc.append(p,q);
    if (q==unesc.end()) {
      break;
    }
    if (*q == ' ') {
      esc += '+';
    } else {
      esc += format("%%%02x",static_cast< boost::uint8_t>(*q));
    }      
    p = q+1;
  }
  return esc;
}


};

