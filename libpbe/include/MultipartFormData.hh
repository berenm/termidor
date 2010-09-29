// include/MultipartFormData.hh
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

#ifndef libpbe_MultipartFormData_hh
#define libpbe_MultipartFormData_hh

#include "Exception.hh"
#include "predicates.hh"
#include "caseconv.hh"

#include <string>
#include <map>
#include <algorithm>


// Parse multipart/form-data MIME, as used for file upload in HTTP POST.
// Should probably be decomposed into a "mutipart" layer with a "form-data" 
// layer on top.
// The boundary is specified in the main HTTP header, e.g.
// Content-Type: multipart/form-data; boundary="abcdefg"
// This boundary string must be passed to this class's constructor.
// The multipart content then looks like:
// 
// --abcdef
// Content-Disposition: form-data; name="a"; filename="fn"
// Content-Type: text/plain
//
// blah blah
//
// --abcdef
// Content-Disposition: form-data; name="b"; filename="fn"
// Content-Type: text/plain
//
// foo
//
// --abcdef--
//
// Note that there is a section before the first boundary; this is where 
// a MIME email client would traditionally put content for non-MIME-aware 
// recipients.
// 
// MultipartFormData::Malformed is thrown if the input cannot be parsed.
//
// The Part structs contain pointers into the original data, not copies of 
// it.  For this to work, the original data must still be valid as long as 
// the Parts are being accessed.


namespace pbe {


class MultipartFormData {

public:
  struct Part {
    std::string filename;
    const char* data_begin;
    const char* data_end;
    Part() {}
    template <typename ITER>
    Part(std::string filename_, ITER data_begin, ITER data_end):
      filename(filename_), data_begin(&(*data_begin)), data_end(&(*data_end))
    {}
  };

private:
  typedef std::map<std::string,Part> parts_t;
  parts_t parts;

  template <typename ITER>
  void init(ITER data_begin, ITER data_end, std::string boundary) {
    std::string first_bpattern("--"+boundary);
    ITER p = std::search(data_begin,data_end,first_bpattern.begin(),first_bpattern.end());
    if (p==data_end) {
std::cout << "no first boundary found";
      throw Malformed();
    }
    p = p+first_bpattern.size();

    std::string bpattern("\r\n--"+boundary);

    while (p != data_end) {
      if (std::string(p,p+2)=="--") {
        break;
      }
      ITER q = std::search(p,data_end,bpattern.begin(),bpattern.end());
      // FIXME presumably this was to cope with some misformatted input:
      // while (is_any_of<' ','\t'>(*p)) {
      //   ++p;
      // }
      if (std::string(p,p+2)!="\r\n") {
std::cout << "No newline\n";
        throw Malformed();
      }
      p += 2;
      std::string crlfcrlf("\r\n\r\n");
      ITER blank_line_pos = std::search(p,q,crlfcrlf.begin(),crlfcrlf.end());
      if (blank_line_pos==q) {
std::cout << "No blank line\n";
        throw Malformed();
      }
    
      std::string name="";
      std::string filename="";

      while (p != blank_line_pos) {
        std::string crlf("\r\n");
        ITER h = std::search(p,blank_line_pos,crlf.begin(),crlf.end());
        MimeHeader hdr(p,h);
        if (tolower(hdr.name())=="content-disposition"
            && tolower(hdr.value())=="form-data") {
	  name = hdr.param("name");
	  filename = hdr.param("filename");
	}
        if (tolower(hdr.name())=="content-transfer-encoding") {
          throw "Content transfer encodings are not supported";
        }
        if (h==blank_line_pos) {
          break;
        }
        p = h+crlf.size();
      }

      if (name.empty()) {
std::cout << "Part has no name\n";
        throw Malformed();
      }

      parts.insert(make_pair(name,Part(filename,blank_line_pos+crlfcrlf.size(),q)));

      p = q+bpattern.size();
    }
  }

public:

  struct Malformed: public StrException {
    Malformed(): StrException("Malformed MultipartFormData") {}
  };

  struct NoSuchPart: public StrException {
    NoSuchPart(): StrException("No such part") {}
  };

  template <typename ITER>
  MultipartFormData(ITER data_begin, ITER data_end, std::string boundary) {
    init(data_begin,data_end,boundary);
  }

  // This is not valid because the parts will keep pointers to the contents of data(),
  // which is a temporary.
  // MultipartFormData(std::string data, std::string boundary) {
  //   init(data.begin(),data.end(),boundary);
  // }

  const Part& part(std::string name) const {
    parts_t::const_iterator i = parts.find(name);
    if (i==parts.end()) {
      throw NoSuchPart();
    } else {
      return i->second;
    }
  }
};



};

#endif

