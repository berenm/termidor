// src/LineFile.cc
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

#include "LineFile.hh"

#include "starts_ends_with.hh"


using namespace std;


namespace pbe {


LineFile::LineFile(const char* fn):
  f(fn)
{
  get_next_line();
  linenum_ = 0;
}

LineFile::LineFile(std::string fn):
  f(fn.c_str())
{
  get_next_line();
  linenum_ = 0;
}

void LineFile::get_next_line()
{
  std::getline(f,next_line);
  ++linenum_;
  at_eof = f.eof();
  if (ends_with(next_line,'\r')) {
    next_line = string(next_line.begin(),next_line.end()-1);
  }
}

std::string LineFile::getline()
{
  string s = next_line;
  get_next_line();
  return s;
}

bool LineFile::eof() const
{
  return at_eof;
}


};






