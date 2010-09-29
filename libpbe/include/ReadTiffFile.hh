// include/ReadTiffFile.hh
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

#ifndef libpbe_ReadTiffFile_hh
#define libpbe_ReadTiffFile_hh

#include <tiffio.h>

#include <boost/noncopyable.hpp>

#include <stdint.h>


// ReadTiffFile is a wrapper around libtiff's facilities for reading
// a TIFF file.  The ctor takes a filename; you can then access properties
// such as the dimensions of the image.
//
// libtiff provides various methods to access the pixel data, some of
// which work with all files and others work with files with particular
// organisations [tiff allows data to be in raster order or tiled, with
// the colour channels packed or separate, etc].  This wrapper currently
// only supports reading files that use "ARGB strips", i.e. the colours 
// are decoded to 32-bits-per-pixel and the data is in strips the full 
// width of the image and some some number of pixels (possibly 1) high,
// or whole images.
//
// Boost.GIL also has a libtiff wrapper, but I believe that it only
// supports reading whole images.


namespace pbe {


struct ReadTiffFile: boost::noncopyable {
  TIFF* const tiff;
  ReadTiffFile(const char* fn):
    tiff(TIFFOpen(fn,"r"))
  {
    if (!tiff) {
      throw "TIFFOpen() failed";
    }
  }
  ReadTiffFile(TIFF* tiff_):
    tiff(tiff_)
  {}
  ~ReadTiffFile() {
    TIFFClose(tiff);
  }

  template <typename T>
  T get_tag(ttag_t tag) const {
    T val;
    int r = TIFFGetField(tiff,tag,&val);
    if (r==0) {
      throw "TIFFGetField() failed";
    }
    return val;
  }

  template <typename T>
  T get_tag_array(ttag_t tag) const {
    T val;
    int r = TIFFGetField(tiff,tag,&val);
    if (r==0) {
      throw "TIFFGetField() failed";
    }
    return val;
  }

  uint32_t width() const {
    return get_tag<uint32_t>(TIFFTAG_IMAGEWIDTH);
  }
  uint32_t height() const {
    return get_tag<uint32_t>(TIFFTAG_IMAGELENGTH);
  }
  uint32_t rows_per_strip() const {
    return get_tag<uint32_t>(TIFFTAG_ROWSPERSTRIP);
  }
  size_t number_of_strips() const {
    return TIFFNumberOfStrips(tiff);
  }

  void read_rgba_strip(uint32_t row, uint32_t* data) const {
    int r = TIFFReadRGBAStrip(tiff,row,data);
    if (r==0) {
      throw "TIFFReadRGBAStrip failed";
    }
  }

  void read_rgba_image(uint32_t* data) const {
    int r = TIFFReadRGBAImage(tiff,width(),height(),data,0);  // 0 = don't stop immediately on error
    if (r==0) {
      throw "TIFFReadRGBAImage() failed";
    }
  }
};


};


#endif

