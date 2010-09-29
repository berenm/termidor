// include/MimeHeader.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe
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

#ifndef libpbe_MimeHeader_hh
#define libpbe_MimeHeader_hh

#include "Exception.hh"
#include "caseconv.hh"

#include <string>
#include <map>
#include <algorithm>
#include <cctype>


// MimeHeader represents one line of the headers of a MIME entity (e.g.
// email, HTTP request or response, etc.).  Typically such a line looks 
// like:
// Header-Name: value; param=val; param2=val2
// For example:
// Content-Type: text/html; charset="UTF-8"

// I am unsure exactly where whitespace may appear.  This strips it from 
// around the values.
// The param value may or may not be quoted.
// This fails if the param value contains a ';', even if it is quoted.
// Components of the header are often case-insensitive.  Dealing with this 
// is the caller's responsibility, except that lookups of parameters are 
// made case-insensitive here.  Looking for a non-existent parameter 
// returns an empty string.
// MimeHeader::Malformed is thrown if the input cannot be parsed.


namespace pbe {


struct compare_insens {
  inline bool operator()(std::string a, std::string b) const {
    return tolower(a) < tolower(b);  // FIXME could do case conversion lazily
  }
};

template <typename ITER>
std::string trim_whitespace(ITER begin, ITER end) {
  while (begin != end && isspace(*begin)) ++begin;
  while (begin != end && isspace(*(end-1))) --end;
  return std::string(begin,end);
}


class MimeHeader {
private:

  std::string name_;
  std::string value_;
  typedef std::map<std::string,std::string,compare_insens> params_t;
  params_t params;

  template <typename ITER>
  void init(ITER begin, ITER end) {
    ITER colon_pos = std::find(begin,end,':');
    if (colon_pos==end) {
      throw Malformed();
    }
    name_ = std::string(begin,colon_pos);
    ITER semi_pos = std::find(colon_pos+1,end,';');
    value_ = trim_whitespace(colon_pos+1,semi_pos);
    ITER p = semi_pos;
    while (p != end) {
      ++p;
      ITER q = std::find(p,end,';');
      ITER equals_pos = std::find(p,q,'=');
      if (equals_pos==q) {
        throw Malformed();
      }
      std::string pname = trim_whitespace(p,equals_pos);
      std::string pvalue = trim_whitespace(equals_pos+1,q);
      if (pvalue[0]=='\"' && pvalue[pvalue.size()-1]=='\"') {
	pvalue=pvalue.substr(1,pvalue.size()-2);
      }
      params.insert(make_pair(pname,pvalue));
      p = q;
    }
  }

public:

  struct Malformed: public StrException {
    Malformed():
      StrException("Malformed MIME header")
    {}
  };

  template <typename ITER>
  MimeHeader(ITER begin, ITER end) {
    init(begin,end);
  }

  MimeHeader(const std::string s) {
    init(s.begin(),s.end());
  }

  std::string name(void) const { return name_; }
  std::string value(void) const { return value_; }
  std::string param(std::string name) const {
    params_t::const_iterator i = params.find(name);
    if (i==params.end()) {
      return "";
    } else {
      return i->second;
    }
  }
};


};


#endif

