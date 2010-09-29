// include/mk_xml.hh
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

#ifndef libpbe_mk_xml_hh
#define libpbe_mk_xml_hh

#include <string>

#include "xml_utils.hh"
#include "to_str.hh"


namespace pbe {


template <typename T>
class attribute_class {
  std::string name;
  T value;
public:
  attribute_class(std::string name_, T value_): name(name_), value(value_) {}
  std::string str() const { return ' ' + name + "=\"" + escape_quotes_for_xml(to_str(value)) + '"'; }
};

template <typename T>
attribute_class<T> attribute(std::string name, T value) { return attribute_class<T>(name,value); }


class element {
  std::string name;
  std::string attributes;
  std::string content;

public:
  element(std::string name_): name(name_) {}

  template <typename T>
  element(std::string name_, const T& content_): name(name_), content(escape_for_xml(to_str(content_))) {}

  template <typename T>
  element& operator<<(const attribute_class<T>& attr) { attributes += attr.str(); return *this; }

  element& operator<<(const element& e) { content += e.str(); return *this; }

  template <typename T>
  element& operator<<(const T& t) { content += escape_for_xml(to_str(t)); return *this; }

  std::string str() const {
    if (content.empty()) {
      return '<' + name + attributes + "/>";
    } else {
      return '<' + name + attributes + '>' + content + "</" + name + '>';
    }
  }

//  friend ostream& operator<<(ostream& strm, const element& elem);
};

//ostream& operator<<(ostream& strm, const element& elem) {
//  return strm << elem.str();
//}


};


#endif
