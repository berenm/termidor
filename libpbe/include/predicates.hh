// include/predicates.hh
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

#ifndef libpbe_predicates_hh
#define libpbe_predicates_hh

namespace pbe {

template <char c1>
bool is_any_of(char c) {
  return c==c1;
}

template <char c1, char c2>
bool is_any_of(char c) {
  return c==c1 || c==c2;
}

template <char c1, char c2, char c3>
bool is_any_of(char c) {
  return c==c1 || c==c2 || c==c3;
}

template <char c1, char c2, char c3, char c4>
bool is_any_of(char c) {
  return c==c1 || c==c2 || c==c3 || c==c4;
}

template <char c1, char c2, char c3, char c4, char c5>
bool is_any_of(char c) {
  return c==c1 || c==c2 || c==c3 || c==c4 || c==c5;
}

template <char c1, char c2, char c3, char c4, char c5, char c6>
bool is_any_of(char c) {
  return c==c1 || c==c2 || c==c3 || c==c4 || c==c5 || c==c6;
}

template <char c1, char c2, char c3, char c4, char c5, char c6, char c7>
bool is_any_of(char c) {
  return c==c1 || c==c2 || c==c3 || c==c4 || c==c5 || c==c6 || c==c7;
}

template <char c1, char c2, char c3, char c4, char c5, char c6, char c7, char c8>
bool is_any_of(char c) {
  return c==c1 || c==c2 || c==c3 || c==c4 || c==c5 || c==c6 || c==c7 || c==c8;
}


};


#endif




