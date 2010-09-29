// include/LineFile.hh
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

#ifndef libpbe_LineFile_hh
#define libpbe_LineFile_hh


// Read files that are organised into lines.
// Implemented on top of ifstream.
// Key thing is that eof() on a plain ifstream tells you that you've 
// reached eof only after you've tried to read past the end.  This hides 
// that, and eof() returns true as soon as you've read the last line. 
// To achieve this it has to cache the next line.
// Files with LF and CRLF line endings are understood; the caller doesn't 
// see any of these characters.

#include <fstream>
#include <string>

namespace pbe {


class LineFile {

  std::ifstream f;
  bool at_eof;
  std::string next_line;
  unsigned int linenum_;
  void get_next_line();

public:
  LineFile(const char* fn);
  LineFile(std::string fn);

  std::string getline();
  bool eof() const;
  unsigned int linenum() const { return linenum_; }

};


};


#endif




