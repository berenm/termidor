// include/bithacks.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
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

#ifndef libpbe_bithacks_hh
#define libpbe_bithacks_hh


// Bit-twiddling fun.
// See e.g. http://bits.stephan-brumme.com/, http://graphics.stanford.edu/~seander/bithacks.html

namespace pbe {


inline bool is_pot(uint32_t n)
{
  return (n & (n-1)) == 0;
}


inline uint32_t round_up_pot(uint32_t n)
{
  // Returns the smallest power of 2 >= n.
  --n;  // So the ==pot case works.
  // "Spread out" bits till eventually the MSbit is spread right down to bit 0:
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  // Now n is one less than a pot.
  ++n;
  return n;
}


};


#endif

