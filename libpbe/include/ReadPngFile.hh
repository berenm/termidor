// include/ReadPngFile.hh
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

#ifndef libpbe_ReadPngFile_hh
#define libpbe_ReadPngFile_hh

#include <png.h>

#include <boost/noncopyable.hpp>


// ReadPngFile is a wrapper around libpng's facilities for reading
// a PNG file.  The ctor takes a filename; you then obtain pixel data
// row-at-a-time.  This is a small subsit of libpng's facilities.
//
// Boost.GIL also has a libpng wrapper, but I believe that it only
// supports reading whole images.


namespace pbe {


struct ReadPngFile {

  FILE* f;
  png_structp png_p;
  png_infop info_p;

  png_uint_32 width;
  png_uint_32 height;
  int bit_depth;
  int colour_type;

  static FILE* check_fopen(const char* fn, const char* mode) {
    FILE* f = fopen(fn,mode);
    if (!f) {
      throw "fopen() failed";
    }
    return f;
  }

  static png_structp check_png_create_read_struct(png_const_charp user_png_ver, png_voidp error_ptr,
                                                  png_error_ptr error_fn, png_error_ptr warn_fn) {
    png_structp p = png_create_read_struct(user_png_ver, error_ptr, error_fn, warn_fn);
    if (!p) {
      throw "png_create_read_struct() failed";
    }
    return p;
  }

  static png_infop check_png_create_info_struct(png_structp png_ptr) {
    png_infop i = png_create_info_struct(png_ptr);
    if (!i) {
      throw "png_create_info_struct() failed";
    }
    return i;
  }

  static void error_fn(png_structp, png_const_charp error_msg) {
    throw error_msg;
  }

  ReadPngFile(const char* fn):
    f(check_fopen(fn,"rb")),
    png_p(check_png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, &error_fn, 0)),
    info_p(check_png_create_info_struct(png_p))
  {
    png_byte header[8];
    fread(header,1,sizeof(header),f);
    if (png_sig_cmp(header,0,sizeof(header))) {
      throw "Not a PNG file";
    }
    png_init_io(png_p,f);
    png_set_sig_bytes(png_p,sizeof(header));
    png_read_info(png_p, info_p);
    png_get_IHDR(png_p, info_p, &width, &height, &bit_depth, &colour_type,
                                NULL, NULL, NULL);
  }

  ~ReadPngFile() {
    // Ignore any errors that occur.  Unlike WritePngFile, in this case it probably
    // doesn't matter much.
    try { png_read_end(png_p, NULL); } catch (...) {}
    try { png_destroy_read_struct(&png_p,&info_p,NULL); } catch (...) {}  // frees both
    fclose(f);
  }

  void read_row(uint32_t* data) {
    png_read_row(png_p, reinterpret_cast<png_byte*>(data), NULL);
  }

};


};


#endif

