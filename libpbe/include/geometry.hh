// include/geometry.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
// (C) 2008 Philip Endecott

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

#ifndef libpbe_geometry_hh
#define libpbe_geometry_hh

#include "moremaths.hh"

#include <cmath>


namespace pbe {

const float earth_radius = 6372795.0F;

inline float normalise_unsigned_angle(float a) { return a>0 ? fmod(a,360.0F) : 360.0F+fmod(a,360.0F); }
inline float normalise_signed_angle(float a) { return normalise_unsigned_angle(a+180.0F)-180.0F; }


struct position {
  float lng;
  float lat;
  float alt;

  position() {}
  position(float lng_, float lat_, float alt_):
    lng(lng_), lat(lat_), alt(alt_)
  {}
};

float distance(const position& a, const position& b);        // Proper great circle calculation
float bearing(const position& a, const position& b);
float elevation(const position& a, const position& b);

inline float cheap_distance(const position& a, const position& b) {
  return earth_radius / 360.0F * 2*PI
         * sqrt( sqr(a.lat-b.lat) + sqr((a.lng-b.lng)*degcos(a.lat)) );
}

inline float cheap_bearing(const position& a, const position& b) {
  float d = degatan2( (b.lng-a.lng) * degcos(a.lat),
                      b.lat-a.lat );
  return normalise_unsigned_angle(d);
}

inline float elevation(const position& a, const position& b) {
  return degatan2(b.alt-a.alt,cheap_distance(a,b));
}

inline float rectilinear_elevation(const position& a, const position& b) {
  return rad2deg((b.alt-a.alt) / cheap_distance(a,b));
}

};


#endif

