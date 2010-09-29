// include/strto.hh
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

#ifndef libpbe_strto_hh
#define libpbe_strto_hh

#include <stdio.h>
#include <string>
#include <cctype>
#include <algorithm>


namespace pbe {


template <typename ITER>
long int strtol(ITER p, ITER q)
{
  while (p<q && *p==' ') {
    ++p;
  }
  if (p==q) {
    throw "strtol failed";
  }
  unsigned long r=0;
  int sign=1;
  if (*p=='-') {
    sign=-1;
    ++p;
  }
  while (p<q && *p!=' ') {
    if (!isdigit(*p)) {
      throw "strtol failed";
    }
    r = 10*r + (*p - '0');
    ++p;
  }
  while (p<q && *p==' ') {
    ++p;
  }
  if (p==q) {
    return r * sign;
  }
  throw "strtol failed";
}

template <int N>
long int strtol(const char (&s) [N])
{
  return strtol(&s[0],&s[N]);
}

inline long int strtol(std::string s)
{
  return strtol(s.begin(),s.end());
}


template <typename ITER>
long int hex_strtol(ITER p, ITER q)
{
  while (p<q && *p==' ') {
    ++p;
  }
  if (p==q) {
    throw "hex_strtol failed";
  }
  unsigned long r=0;
  int sign=1;
  if (*p=='-') {
    sign=-1;
    ++p;
  }
  while (p<q && *p!=' ') {
    if (!isxdigit(*p)) {
      throw "hex_strtol failed";
    }
    r = 16*r;
    if (isdigit(*p)) {
      r += *p - '0';
    } else {
      r += std::tolower(*p) - 'a' + 10;
    }
    ++p;
  }
  while (p<q && *p==' ') {
    ++p;
  }
  if (p==q) {
    return r * sign;
  }
  throw "strtol failed";
}

template <int N>
long int hex_strtol(const char (&s) [N])
{
  return hex_strtol(&s[0],&s[N]);
}

inline long int hex_strtol(std::string s)
{
  return hex_strtol(s.begin(),s.end());
}


template <typename ITER>
double strtod(ITER p, ITER q)
{
  std::string s(p,q);
  std::string::iterator d_pos = std::find(s.begin(),s.end(),'D');
  if (d_pos != s.end()) {
    *d_pos = 'e';
  }
  double d;
  int r = sscanf(s.c_str(),"%lf",&d);
  if (r!=1) {
    throw "strtod failed";
  }
  return d;
}

template <int N>
double strtod(const char (&s) [N])
{
  return strtod(&s[0],&s[N]);
}

inline double strtod(std::string s)
{
  return strtod(s.begin(),s.end());
}


template <typename ITER>
float strtof(ITER p, ITER q)
{
  std::string s(p,q);
  float f;
  int r = sscanf(s.c_str(),"%f",&f);
  if (r!=1) {
    throw "strtof failed";
  }
  return f;
}

template <int N>
float strtof(const char (&s) [N])
{
  return strtof(&s[0],&s[N]);
}

inline float strtof(std::string s)
{
  return strtof(s.begin(),s.end());
}

};


#endif


