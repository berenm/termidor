// csv.hh
// (C) 2008 Philip Endecott
// This file is part of libpbe.  See http://svn.chezphil.org/libpbe/
// This file is distributed under the terms of the Boost Software License v1.0.
// Please see http://www.boost.org/LICENSE_1_0.txt or the accompanying file BOOST_LICENSE.

#ifndef pbe_csv_hh
#define pbe_csv_hh

#include <string>
#include <vector>

namespace pbe {

// Parse a line from a comma-separated-variables (CSV) file.
// The fields are used to populate a vector.
//
// Each field may be quoted with "" or not quoted.  If it's quoted it may
// contain unescaped commas and "s represented by ""; if it's not quoted it can't.  
//
// The code makes some attempt to handle invalid input sanely, but
// it is not designed to be secure against malicious input.

typedef std::vector<std::string> csv_columns_t;

extern void parse_csv_line(std::string l, csv_columns_t& v);


// Escape quotes, for output:

extern std::string escape_quotes_for_csv(std::string s);


};

#endif
