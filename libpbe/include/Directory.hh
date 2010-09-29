// src/Directory.hh
// This file is part of libpbe; see http://decimail.org
// (C) 2004 - 2006 Philip Endecott

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


// Provides a way to iterate through the files and subdirectories within a 
// directory.  It is somewhat like an STL Input Container, but is missing 
// many of the required methods.
//
// The . and .. directory entries are skipped, avoiding one of the ways of 
// going mad, but symbolic links still provide a way to confuse yourself.
//
// After copying a const_iterator, one or other copy may be freely used but 
// not both, since there is only a single underlying DIR* object.


#ifndef libpbe_Directory_hh
#define libpbe_Directory_hh

#include <boost/shared_ptr.hpp>

#include <sys/types.h>
#include <dirent.h>

#include <string>

namespace pbe {

class Directory {

public:
  Directory(std::string dirname_);
  ~Directory();

  struct Entry {
    std::string leafname;
    std::string pathname;
  };

  class const_iterator {
  public:
    typedef std::forward_iterator_tag iterator_category;
    typedef Entry value_type;
    typedef int difference_type;
    typedef Entry* pointer;
    typedef Entry& reference;

    void operator++();
    Entry operator*() const {return this_entry;}
    const Entry* operator->() const {return &this_entry;}
    bool operator==(const const_iterator& r) const;
    bool operator!=(const const_iterator& r) const { return !((*this)==r); }
    friend class Directory;
  private:
    std::string dirname;
    boost::shared_ptr<DIR> dir;
    bool at_end;
    Entry this_entry;
    const_iterator();
    const_iterator(std::string dirname_);
    void read_next();
  };

  const_iterator begin() const;
  const_iterator end() const;

  size_t size() const;

private:
  std::string dirname;

};

};

#endif
