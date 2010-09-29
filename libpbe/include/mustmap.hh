// include/mustmap.hh
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

#ifndef libpbe_mustmap_hh
#define libpbe_mustmap_hh


// pbe::mustmap<K,V> is an a container like std::map<K,V> except that it
// throws an exception, pbe::notfound, if operator[] fails to find an
// existing element with the requested key.
// So operator[] cannot be used for insertion; you must use insert() for
// that.  But operator[] can be used for access in cases where you would
// otherwise have needed to use find().


#include <map>
#include <functional>
#include <memory>

namespace pbe {

struct notfound {};


template <typename K, typename V, typename COMP=std::less<K>,
          typename ALLOC=std::allocator< std::pair<const K, V> > >
class mustmap: public std::map<K,V,COMP,ALLOC> {

  typedef std::map<K,V,COMP,ALLOC> base_t;

public:
  mustmap() {}
  mustmap(const COMP& comp): base_t(comp) {}
  template <class InputIterator>
  mustmap(InputIterator f, InputIterator l): base_t(f,l) {}
  template <class InputIterator>
  mustmap(InputIterator f, InputIterator l, const COMP& comp): base_t(f,l,comp) {}
  mustmap(const mustmap& m): base_t(m) {}

  V& operator[](const K& k) {
    typename base_t::iterator i = base_t::find(k);
    if (i==base_t::end()) {
      throw notfound();
    }
    return i->second;
  }

  const V& operator[](const K& k) const {
    typename base_t::const_iterator i = base_t::find(k);
    if (i==base_t::end()) {
      throw notfound();
    }
    return i->second;
  }

};


};


#endif



