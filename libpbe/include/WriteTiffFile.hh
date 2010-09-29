// include/WriteTiffFile.hh
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

#ifndef libpbe_WriteTiffFile_hh
#define libpbe_WriteTiffFile_hh

#include <tiffio.h>

#include <boost/noncopyable.hpp>

#include <stdint.h>


// WriteTiffFile is a wrapper around libtiff's facilities for writing
// a TIFF file.  The ctor takes a filename, image dimensions, ...
//
// libtiff provides various methods to provide the pixel data and store 
// it in the file.  This wrapper currently only supports writing files 
// that use "ARGB strips", i.e. the colours are encoded from 32-bits-per-pixel 
// and the data is in strips the full width of the image and some some 
// number of pixels (possibly 1) high.  You can write either single strips 
// or a whole image made of strips.
//
// Boost.GIL also has a libtiff wrapper, but I believe that it only
// supports writing whole images.


namespace pbe {


struct WriteTiffFile: boost::noncopyable {
  TIFF* const tiff;
  WriteTiffFile(const char* fn, size_t w, size_t h):
    tiff(TIFFOpen(fn,"w"))
  {
    if (!tiff) {
      throw "TIFFOpen() failed";
    }
    set_default_tags(w,h);
  }
  WriteTiffFile(TIFF* tiff_, size_t w, size_t h):
    tiff(tiff_)
  {
    set_default_tags(w,h);
  }
  ~WriteTiffFile() {
    TIFFClose(tiff);
  }

  void set_default_tags(size_t w, size_t h) {
    set_tag<uint32_t>(TIFFTAG_IMAGEWIDTH,w);
    set_tag<uint32_t>(TIFFTAG_IMAGELENGTH,h);
    set_tag<uint16_t>(TIFFTAG_BITSPERSAMPLE,8);
    set_tag<uint16_t>(TIFFTAG_SAMPLESPERPIXEL,4);
    set_tag<uint16_t>(TIFFTAG_COMPRESSION,  COMPRESSION_PACKBITS);
    set_tag<uint16_t>(TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    set_tag<uint16_t>(TIFFTAG_PHOTOMETRIC,  PHOTOMETRIC_RGB);
    set_tag<uint16_t>(TIFFTAG_ORIENTATION,  ORIENTATION_TOPLEFT);
    set_tag<uint32_t>(TIFFTAG_ROWSPERSTRIP,TIFFDefaultStripSize(tiff,1));
  }

  template <typename T>
  void set_tag(ttag_t tag, T val) {
    int r = TIFFSetField(tiff,tag,val);
    if (r==0) {
      throw "TIFFSetField() failed";
    }
  }

  uint32_t width() const {
    uint32_t w;
    int r = TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&w);
    if (r==0) {
      throw "TIFFGetField(TIFFTAG_IMAGEWIDTH); failed";
    }
    return w;
  }

  uint32_t height() const {
    uint32_t h;
    int r = TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&h);
    if (r==0) {
      throw "TIFFGetField(TIFFTAG_IMAGLENGTH); failed";
    }
    return h;
  }

  void write_rgba_row(uint32_t row, const uint32_t* data) {
    int r = TIFFWriteScanline(tiff,const_cast<uint32_t*>(data),row);
    if (r==-1) {
      throw "TIFFWriteScanline failed";
    }
  }

  void write_rgba_image(uint32_t* data) {
    uint32_t w = width();
    uint32_t h = height();
    data += w*h;
    for (unsigned int row=0; row<h; ++row) {
      data -= w;
      write_rgba_row(row,data);
    }
  }
};


};


#endif

