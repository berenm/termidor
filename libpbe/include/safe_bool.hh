// include/safe_bool.hh
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

#ifndef libpbe_safe_bool_hh
#define libpbe_safe_bool_hh

namespace pbe {

// See e.g. http://www.artima.com/cppsource/safebool.html for the 
// background.
// A class that wants an "operator safe_bool() const" should inherit from this 
// CRTP base class, and implement a (private) method "bool boolean_test() const" 
// that does the work.
// This provides only the CRTP version, not the virtual version also 
// presented at the page linked above.


template <typename DERIVED>
class safe_bool {
protected:
  typedef void (safe_bool::*bool_type)() const;
  void this_type_does_not_support_comparisons() const {}
  safe_bool() {}
  safe_bool(const safe_bool&) {}
  safe_bool& operator=(const safe_bool&) {return *this;}
  ~safe_bool() {}
public:
  operator bool_type() const {
    return (static_cast<const DERIVED*>(this))->boolean_test()
         ? &safe_bool::this_type_does_not_support_comparisons : 0;
  }
};


#if 0

template <typename T, typename U> 
void operator==(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
  lhs.this_type_does_not_support_comparisons();	
  return false;
}

template <typename T,typename U> 
void operator!=(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
  lhs.this_type_does_not_support_comparisons();
  return false;	
}

#endif

};


#endif




