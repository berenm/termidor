// src/macroman.cc
// This file is part of libpbe; see http://decimail.org
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

// This work is based on Boost.Array, which is:
// (C) Copyright Nicolai M. Josuttis 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "macroman.hh"

#include <iostream>

#include "charset/utf8.hh"
#include "charset/sequence_converter.hh"
#include "charset/conv/macintosh.hh"
#include "charset/conv/unicode.hh"
#include "charset/const_character_iterator.hh"


using namespace std;


namespace pbe {


static bool is_ascii(string s)
{
  for (string::const_iterator i = s.begin();
       i != s.end(); ++i) {
    const uint8_t u = static_cast<uint8_t>(*i);
    if (u&0x80) {
      return false;
    }
  }
  return true;
}


string utf8_to_macroman(string s)
{
  if (is_ascii(s)) {
    return s;
  }

  string r;

  typedef const_character_iterator<cs::utf8, string::const_iterator> utf8_iter;
  typedef back_insert_iterator<string> string_back_insert_iter_t;
  sequence_converter<cs::utf8, cs::macintosh,
                     error_policy_approximate< error_policy_return_sentinel<'?'> >,
                     utf8_iter, string_back_insert_iter_t> conv;

  conv(utf8_iter(s.begin()),utf8_iter(s.end()),string_back_insert_iter_t(r));

  return r;
}


};


