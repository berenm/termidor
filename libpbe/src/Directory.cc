// src/Directory.cc
// This file is part of libpbe; see http://decimail.org
// (C) 2004 Philip Endecott

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

#include "Directory.hh"

#include "Exception.hh"

#include <unistd.h>
#include <algorithm>

using namespace std;

namespace pbe {


Directory::Directory(string dirname_):
  dirname(dirname_)
{}


Directory::~Directory()
{}


void Directory::const_iterator::operator++()
{
  read_next();
}


bool Directory::const_iterator::operator==(const const_iterator& rhs) const
{
  if (!at_end && !rhs.at_end) {
    throw "Cannot compare these Directory::const_iterators";
  }
  return at_end==rhs.at_end;
}


Directory::const_iterator::const_iterator():
  at_end(true)
{}


Directory::const_iterator::const_iterator(string dirname_):
  dirname(dirname_),
  dir(opendir(dirname.c_str()), &closedir),
  at_end(false)
{
  if (!dir) {
    throw_ErrnoException("opendir("+dirname+")");
  }
  read_next();
}


void Directory::const_iterator::read_next()
{
  struct dirent ent;
  struct dirent* ent_p;
  int r = readdir_r(dir.get(),&ent,&ent_p);
  if (r!=0 || !ent_p) {
    at_end=true;
  } else {
    this_entry.leafname = ent_p->d_name;
    if (this_entry.leafname=="." || this_entry.leafname=="..") {
      read_next();
      return;
    }
    this_entry.pathname = dirname + '/' + this_entry.leafname;
  }
}


Directory::const_iterator Directory::begin() const
{
  return const_iterator(dirname);
}


Directory::const_iterator Directory::end() const
{
  return const_iterator();
}


size_t Directory::size() const
{
  return std::distance(begin(),end());
}


};

