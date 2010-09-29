// include/moremaths.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
// (C) 2008-2009 Philip Endecott

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

#ifndef libpbe_moremaths_hh
#define libpbe_moremaths_hh

#include <cmath>


namespace pbe {

const float PI = 3.1415927F;
const double PId = 3.1415927;

template <typename T>
inline T sqr(T x) { return x*x; }

template <typename T>
inline T pythag(T x, T y) { return sqrt(sqr(x)+sqr(y)); }

inline float deg2rad(float a) { return PI/180.0F * a; }
inline float rad2deg(float a) { return 180.0F/PI * a; }

inline float degsin(float a) { return sin(deg2rad(a)); }
inline float degcos(float a) { return cos(deg2rad(a)); }
inline float degtan(float a) { return tan(deg2rad(a)); }

inline float degasin(float x) { return rad2deg(asin(x)); }
inline float degacos(float x) { return rad2deg(acos(x)); }
inline float degatan(float x) { return rad2deg(atan(x)); }
inline float degatan2(float y, float x) { return rad2deg(atan2(y,x)); }

inline double deg2rad(double a) { return PId/180.0 * a; }
inline double rad2deg(double a) { return 180.0/PId * a; }

inline double degsin(double a) { return sin(deg2rad(a)); }
inline double degcos(double a) { return cos(deg2rad(a)); }
inline double degtan(double a) { return tan(deg2rad(a)); }

inline double degasin(double x) { return rad2deg(asin(x)); }
inline double degacos(double x) { return rad2deg(acos(x)); }
inline double degatan(double x) { return rad2deg(atan(x)); }
inline double degatan2(double y, double x) { return rad2deg(atan2(y,x)); }


};


#endif

