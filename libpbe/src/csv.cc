// csv.cc
// (C) 2008 Philip Endecott
// This file is part of libpbe.  See http://svn.chezphil.org/libpbe/
// This file is distributed under the terms of the Boost Software License v1.0.
// Please see http://www.boost.org/LICENSE_1_0.txt or the accompanying file BOOST_LICENSE.

#include "csv.hh"

#include <string>
#include <vector>
#include <algorithm>

using namespace std;


namespace pbe {

template <typename iter>
static string parse_csv_field(iter& i, iter j)
{
  while (i != j && *i == ' ') {
    ++i;
  }
  if (i==j) {
    return "";
  }
  string s;
  if (*i == '"') {
    // Quoted:
    ++i;  // initial quote
    while (i != j) {
      if (*i == '"') {
        ++i;
        if (i==j) {
          break;
        }
        if (*i != '"') {
          break;
        }
      }
      s += *i;
      ++i;
    }
  } else {
    // Unquoted:
    while (i != j && *i != ',') {
      s += *i;
      ++i;
    }
  }
  return s;
}


void parse_csv_line(const string l, csv_columns_t& v)
{
  v.clear();
  string::const_iterator i = l.begin();
  while (1) {
    v.push_back(parse_csv_field(i, l.end()));
    if (i==l.end()) {
      break;
    }
    if (*i != ',') {
      throw "expecting a comma";
    }
    ++i;
  }
}


string escape_quotes_for_csv(const string s)
{
  string r;
  string::const_iterator p = s.begin();
  while (1) {
    string::const_iterator q = find(p,s.end(),'"');
    r.append(p,q);
    if (q==s.end()) {
      return r;
    }
    r.append("\"\"");
    ++q;
    p=q;
  }
}


};
