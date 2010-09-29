// include/macroman.hh
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


#ifndef libpbe_macroman_hh
#define libpbe_macroman_hh

#include <string>

namespace pbe {

// Conversion from UTF-8 to the macroman character set, optimised
// for the case where only common-characters are used, with approximation
// for some missing characters.


std::string utf8_to_macroman(std::string s);


};


#endif
