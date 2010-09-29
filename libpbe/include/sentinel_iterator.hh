// include/sentinel_iterator.hh
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

#ifndef libpbe_sentinel_iterator_hh
#define libpbe_sentinel_iterator_hh


// sentinel_iterator
// -----------------
//
// A sentinel iterator is an iterator for a range that uses a sentinel value to
// indicate its end.  The most obvious application is iterating over a null-
// terminated C string, and a typedef const_cstr_iterator is provided for this.
//
// Consider this example of using a C++-style algorithm with a C-style string:
// const char* s = f();  // s is null-terminated.
// const char* s_begin = s;
// const char* s_end = s+strlen(s);
// const char* p = std::find(s_begin,s_end,'x');
//
// The problem with this code is that an O(N) strlen is required even if the 
// 'x' is found right at the start of the string.
// 
// With a sentinel iterator, s_end is default-constructed.  A default-constructed 
// sentinel iterator refers to the end of all strings using that sentinel:
//
// const char* s = f();  // s is null-terminated.
// const_cstr_iterator s_begin = s;       
// const_cstr_iterator s_end;         
// const char* p = std::find(s_begin,s_end,'x').base();
//
// Here, s_begin compares equal to s_end when s_begin refers to the sentinel 
// value, 0.  So the code can operate in O(1) time when the 'x' is found at the 
// start of the string.
// 
// The two template parameters to sentinel_iterator are the underlying iterator 
// and the sentinel value.  The underlying iterator must have a singular value 
// when default-constructed (e.g. NULL); this is used to distinguish an end 
// iterator from any other iterator.  Pointers are the most obvious underlying 
// iterators.
// FIXME what std iterators have singular values when default constructed?
// 
// Since the distance to an end iterator is not available, sentinel_iterators 
// cannot be random access iterators.  This is unfortunate since random access 
// operations are possible for all values except end values.
//
// Sentinel operators also provide a conversion to a safe_bool type that indicates 
// whether they have reached the end of their range.
//
//
// predicate_iterator
// ------------------
//
// A generalisation of the sentinel_iterator is the predicate_iterator, which 
// uses an arbitrary predicate to test a value to determine whether it indicates 
// the end of a range.
//
// sentinel_iterator is, in fact, implemented using predicate_iterator.
//
// boost::iterator_facade is used for the implementation of predicate_iterator.


#include "safe_bool.hh"
#include <boost/iterator/iterator_facade.hpp>
#include <iterator>


namespace pbe {


template< typename ITER, typename PRED_T >
class const_predicate_iterator:
  public boost::iterator_facade< const_predicate_iterator<ITER,PRED_T>,
                                 typename std::iterator_traits<ITER>::value_type,
                                 std::bidirectional_iterator_tag,  // FIXME should be fordward if ITER is forward, etc.
                                 const typename std::iterator_traits<ITER>::reference,
                                 typename std::iterator_traits<ITER>::difference_type >,
  safe_bool< const_predicate_iterator<ITER,PRED_T> >
{
  ITER i;
  PRED_T pred;

  const typename std::iterator_traits<ITER>::reference dereference() const { return *i; }
  bool equal(const const_predicate_iterator& other) const {
    return (i==other.i)
        || (!i && other.i && other.pred(*(other.i)))
        || (i && !(other.i) && pred(*i)) ;
  }
  void increment() { ++i; }
  void decrement() { --i; }
  
  friend class boost::iterator_core_access;

public:
  const_predicate_iterator(): i(), pred() {}
  const_predicate_iterator(PRED_T pred_): i(), pred(pred_) {}
  explicit const_predicate_iterator(const ITER& i_): i(i_), pred() {}
  explicit const_predicate_iterator(const ITER& i_, PRED_T pred_): i(i_), pred(pred_) {}
  ITER& base() { return i; }
  bool boolean_test() const { return !pred(*i); }

};


template <typename T, T val>
struct equals {
  bool operator()(T a) const { return a==val; }
};

template< typename ITER, typename std::iterator_traits<ITER>::value_type sentinel >
class const_sentinel_iterator:
  public const_predicate_iterator< ITER, equals<typename std::iterator_traits<ITER>::value_type,sentinel> >
{
  typedef const_predicate_iterator< ITER, equals<typename std::iterator_traits<ITER>::value_type,sentinel> > base_t;
public:
  const_sentinel_iterator(): base_t() {}
  explicit const_sentinel_iterator(const ITER& i_): base_t(i_) {}

};


typedef const_sentinel_iterator<const char*,'\0'> const_cstr_iterator;


};


#endif

