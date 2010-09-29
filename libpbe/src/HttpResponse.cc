// src/HttpResponse.cc
// This file is part of libpbe; see http://anyterm.org/
// (C) 2005-2008 Philip Endecott

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


#include "HttpResponse.hh"

#include "FileDescriptor.hh"
#include "mustfind.hh"

#include <boost/lexical_cast.hpp>

#include <sstream>
#include <functional>

using namespace std;


namespace pbe {


void HttpResponse::send(FileDescriptor& fd)
{
  const char* begin;
  const char* end;
  if (!body.empty()) {
    begin = body.data();
    end = body.data()+body.length();
  } else {
    begin = body_begin;
    end = body_end;
  }
  size_t sz = end-begin;

  bool connection_close = http_version != "HTTP/1.1";
  ostringstream s;
  s << http_version << ' ' << status_code << ' ' << reason_phrase << "\r\n";
  for (map<string,string>::const_iterator i = headers.begin();
       i != headers.end(); ++i) {
    s << i->first << ": " << i->second << "\r\n";
    if (i->first=="Connection" && i->second=="close") {
      connection_close = true;
    }
  }
  bool chunked = !connection_close;
  if (chunked) {
    s << "Transfer-Encoding: chunked\r\n";
  } else {
    s << "Content-Length: " << sz << "\r\n";
  }
  s << "\r\n";

  if (sz==0) {
    if (chunked) {
      s << "0\r\n\r\n";
    }
    fd.writeall(s.str());
    return;
  }
  if (chunked) {
    s << hex << sz << "\r\n";
  }
  fd.writeall(s.str());
  fd.writeall(begin,sz);
  if (chunked) {
    fd.writeall("\r\n0\r\n\r\n");
  }
}


static string get_line(string& s)
{
  string crlf = "\r\n";
  string::iterator i = mustsearch(s.begin(),s.end(),crlf.begin(),crlf.end());
  string l(s.begin(),i);
  s = string(i+2,s.end());
  return l;
}


HttpResponse::HttpResponse(string s):
  body_begin(NULL),
  body_end(NULL)
{
  const string response_line = get_line(s);
  string::const_iterator sp1 = mustfind(response_line.begin(),response_line.end(),' ');
  http_version = string(response_line.begin(),sp1);
  string::const_iterator sp2 = mustfind(sp1+1,response_line.end(),' ');
  status_code = boost::lexical_cast<int>(string(sp1+1,sp2));
  reason_phrase = string(sp2+2,response_line.end());

  while (1) {
    const string hdr = get_line(s);
    if (hdr.empty()) {
      break;
    }
    string::const_iterator colon_pos = mustfind(hdr.begin(),hdr.end(),':');
    const string hdr_name(hdr.begin(),colon_pos);
    const string hdr_val(colon_pos+1,hdr.end());
    const string trimmed_hdr_val(mustfind_if(hdr_val.begin(),hdr_val.end(),
                                             bind2nd(not_equal_to<char>(),' ')),
                                 hdr_val.end());
    headers[hdr_name] = trimmed_hdr_val;
  }

  body = s;
}


};

