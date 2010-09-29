// include/xml_utils.hh
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


// Wrappers around rapidxml to parse a memory-mapped file (including a hideous 
// hack) and utilities to escape text when writing XML.


#ifndef libpbe_xml_utils_hh
#define libpbe_xml_utils_hh

#include <FileDescriptor.hh>
#include <MappedFile.hh>

#include <rapidxml.hpp>

#include <boost/scoped_array.hpp>

#include <string>
#include <algorithm>


namespace pbe {


class xml_from_string: public rapidxml::xml_document<char> {
  boost::scoped_array<char> data;
public:
  xml_from_string(std::string s):
    data(new char[s.length()+1])
  {
    std::copy(s.c_str(),s.c_str()+s.length()+1,data.get());
    parse<0>(data.get());
  }
};



template <bool modify_source>
class xml_file_base: public rapidxml::xml_document<char> {
  pbe::FileDescriptor fd;
  size_t l;
  pbe::MappedFile mf;
public:
  // By default, rapdixml modifies the source to insert \0 string terminators at the 
  // end of each element name, attribute value etc.  It can do this because those things 
  // are always terminated by punctuation e.g. a closing quote, following <, etc.
  // This makes it convenient to access the elements afterwards, but it means that 
  // typically all pages of the memory-mapped file will be copied-on-write.
  // Alternatively, this modification can be disabled, in which case it's necessary to 
  // access the contents using pointer+length pairs.  Even in this case the file is 
  // mapped writable due to the hideous hack.
  xml_file_base(std::string fn):
    fd(fn, pbe::FileDescriptor::read_only),
    l(fd.file_length()),
    mf(fd,l,pbe::FileDescriptor::read_write,0,true)  // Copy on write.
  {
    char* begin = mf.ptr<char>(0);
    *(begin+l-1) = 0;  // Here's the hideous hack: it's essential that the last 
                       // character of the file is e.g. a newline so that we can get 
                       // away with this.
    parse<modify_source ? 0 : rapidxml::parse_non_destructive>(begin);
  }
};


typedef xml_file_base<true> xml_file;

typedef xml_file_base<false> xml_file_nomodify;


// When using xml_file_nomodify, use these to get names and values for nodes and attributes:
template <typename T>
std::string get_name(T* n) {
  const char* begin = n->name();
  return std::string(begin,begin+n->name_size());
}
template <typename T>
std::string get_value(T* n) {
  const char* begin = n->value();
  return std::string(begin,begin+n->value_size());
}


// Use this to escape the content of double-quoted attribute values; 
// "s are replaced by &quot; and &s are replaced by &amp;.
extern std::string escape_quotes_for_xml(const std::string s);

// Use this to escape text nodes; <, > and & are replaced by &lt;, 
// &gt; and &amp; respectively.
extern std::string escape_for_xml(const std::string s);


};


#endif

