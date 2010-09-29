// include/line_algorithms.hh
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

#ifndef libpbe_line_algorithms_hh
#define libpbe_line_algorithms_hh


// Algorithms that operate on (poly-)lines, i.e. sequences of points.

// These use float throughout for things like distances between points, which may not always
// be the right thing to do.


#include <algorithm>
#include <vector>

#include "Vector.hh"


namespace pbe {


// Utilites to measure distances:

inline float sqr(float x) { return x*x; }

template <typename POINT_T>
float sq_distance_from_point_to_point(POINT_T p1, POINT_T p2)
{
  return sqr(p2.x-p1.x) + sqr(p2.y-p1.y);
}

template <typename POINT_T>
float sq_distance_from_point_to_line(POINT_T p, POINT_T l1, POINT_T l2)
{
  // Return the square of the distance from p to the closest point on the
  // line l1-l2.
  // Note that we need the distance to the finite line l1-l2, not to the
  // infinite line passing through l1 and l2.
  // See http://softsurfer.com/Archive/algorithm_0102/algorithm_0102.htm
  // v is the vector from l1 to l2.  w is the vector from l1 to p:
  float vx = l2.x-l1.x;
  float vy = l2.y-l1.y;
  float wx = p.x-l1.x;
  float wy = p.y-l1.y;
  // Find the dot product of v and w.  The sign of this tells us whether p is
  // "in front" or "behind" l1.
  float c1 = vx*wx + vy*wy;
  if (c1<=0) {
    // p is "behind" l1, i.e. l1 is the closest point on l1-l2.
    return sqr(wx) + sqr(wy);
  }
  // Is p closer to l2 than to the line?  I'm not really sure why the
  // following works, but it does and it's less work than doing the 
  // symetric computation.
  float c2 = sqr(vx) + sqr(vy);
  if (c2<=c1) {
    return sq_distance_from_point_to_point(p,l2);
  }
  // p is closest the the line, no to an endpoint.  Find the point that
  // it's closest to.  Again I'm unsure why this is c1/c2, but it is.
  float b = c1/c2;
  POINT_T bp(l1.x + typename POINT_T::coord_t(b*vx),
             l1.y + typename POINT_T::coord_t(b*vy));
  return sq_distance_from_point_to_point(p,bp);
}



// Simplify a line by removing points that are less than minfeaturesize
// from the previous point (vertex reduction).  This is O(n) and can be
// applied as a pre-filter before the more expensive Douglas-Peucker
// algorithm.
// We take care to always retain the first and last points.

template <typename LINE_T>
void vertex_reduce(LINE_T& line, float sqminfeaturesize)
{
  if (line.size() < 2) {
    return;
  }
  LINE_T n;
  typedef typename LINE_T::value_type point_t;
  point_t p = *(line.begin());
  n.push_back(p);
  for (typename LINE_T::const_iterator i = line.begin()+1;
       i != line.end(); ++i) {
    point_t q = *i;
    if (sq_distance_from_point_to_point(p,q) >= sqminfeaturesize) {
      n.push_back(q);
      p = q;
    }
  }
  if (*(n.rbegin()) != *(line.rbegin())) {
    n.push_back(*(line.rbegin()));
  }
  swap(n,line);
}



// Simplify a line by removing points that are less than minfeaturesize
// from the line.  This is the Douglas-Peucker algorithm and is O(nm)
// worst case but O(n log m) typical.  There is an improved algorithm
// that is guaranteed O(n log n) by Hershberger & Snoeyink but it's
// quite a lot more complex.
// We avoid actually modifying the line until the very end, since
// it's a vector with O(n) erase rather than a list, by instead keeping
// a record of the points that we're going to keep.

typedef std::vector<size_t> keep_points_t;

template <typename LINE_T>
void dp_simplify_line_rec(const LINE_T& line, typename LINE_T::const_iterator first,
                          typename LINE_T::const_iterator last,
                          keep_points_t& keep_points, float sqminfeaturesize)
{
  if (std::distance(first,last) < 2) {
    return;
  }
  float maxsqd=-1;
  typename LINE_T::const_iterator maxsqd_i = first;
  for (typename LINE_T::const_iterator i = first+1;
       i != last; ++i) {
    float sqd = sq_distance_from_point_to_line(*i,*first,*last);
    if (sqd>maxsqd) {
      maxsqd=sqd;
      maxsqd_i=i;
    }
  }
  if (maxsqd < sqminfeaturesize) {
    return;
  }
  keep_points.push_back(distance(line.begin(),maxsqd_i));
  dp_simplify_line_rec(line,first,maxsqd_i,keep_points,sqminfeaturesize);
  dp_simplify_line_rec(line,maxsqd_i,last,keep_points,sqminfeaturesize);
}


template <typename LINE_T>
void douglas_peucker(LINE_T& line, float sqminfeaturesize)
{
  keep_points_t keep_points;
  keep_points.push_back(0);
  keep_points.push_back(line.size()-1);
  dp_simplify_line_rec(line,line.begin(),line.end(),keep_points,sqminfeaturesize);

  LINE_T n;
  n.reserve(keep_points.size());
  std::sort(keep_points.begin(),keep_points.end());
  typename LINE_T::const_iterator l = line.begin();
  size_t lpos = 0;
  for (keep_points_t::const_iterator i = keep_points.begin();
       i != keep_points.end(); ++i) {
    advance(l,*i-lpos);
    lpos = *i;
    n.push_back(*l);
  }
  swap(line,n);
}



// Combined function to do vertex reduction followed by Douglas-Peucker.
// As a special case, if the entire line is shorter than the minfeaturesize then
// we truncate it to nothing.

template <typename LINE_T>
void simplify_line(LINE_T& line, float minfeaturesize)
{
  float sqminfeaturesize = sqr(minfeaturesize);

  vertex_reduce(line,sqminfeaturesize);
  douglas_peucker(line,sqminfeaturesize);

  if (line.size()==2) {
    if (sq_distance_from_point_to_point(line[0],line[1]) < sqminfeaturesize) {
      line.clear();
    }
  }
}



// Insert additional points into a line so that no pair of adjacent points is more 
// than max_spacing apart.

template <typename LINE_T>
void interpolate(LINE_T& line, float max_spacing)
{
  if (line.size() < 2) {
    return;
  }
  float sqmaxspacing = sqr(max_spacing);
  LINE_T n;
  typedef typename LINE_T::value_type point_t;
  point_t p = *(line.begin());
  n.push_back(p);
  for (typename LINE_T::const_iterator i = line.begin()+1;
       i != line.end(); ++i) {
    point_t q = *i;
    float sq_distance = sq_distance_from_point_to_point(p,q);
    if (sq_distance > sqmaxspacing) {
      float d = sqrt(sq_distance);
      int n_intervals = static_cast<int>(d / max_spacing) + 1;
      for (int j=1; j<n_intervals; ++j) {
        point_t x = p + (q-p)*(static_cast<float>(j)/n_intervals);
        n.push_back(x);
      }
    }
    n.push_back(q);
    p = q;
  }
  swap(n,line);
}


};


#endif

