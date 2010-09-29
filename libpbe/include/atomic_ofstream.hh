// include/atomic_ofstream.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
// (C) 2008 Philip Endecott

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

#ifndef libpbe_atomic_ofstream_hh
#define libpbe_atomic_ofstream_hh


// atomic_ofstream is like std::ofstream except that the stream writes to
// a temporary file.  When the file has been completely written the client
// code should call commit() which removes any existing file and moves the
// new one into its place; this should happen somewhat atomically.  If
// commit() has not been called by the time the destructor is reached
// the temporary file is removed and the original file is left unchanged.

// The atomicity of rename() is sufficient to give to required atomic
// behaviour from the point of view of a concurrently-executing process
// or in the case of program termination (e.g. ctrl-c).  However, it's no
// sufficient in the case of a system crash; to be safe in this case
// it's necessary to at least call fsync() on the new file before renaming,
// and possibly also to opendir() and fsync() on the containing directory.
// See http://lwn.net/Articles/323169/.  This seems to be tricky in this
// context; it needs a FileDescriptor::ostream but that involes dup(),
// and although it seems to work on Linux it doesn't work on iPhone OS.
// This maybe-safer-but-broken implementation is included below, disabled.


#if 1
// Non-fsync implementation

#include "Exception.hh"

#include <unistd.h>
#include <stdio.h>

#include <fstream>
#include <string>


namespace pbe {


struct atomic_ofstream_base {
  const std::string orig_fn;
  const std::string tmp_fn;
  atomic_ofstream_base(std::string orig_fn_):
    orig_fn(orig_fn_),
    tmp_fn(orig_fn+".tmp")
  {}
};


class atomic_ofstream:
  private atomic_ofstream_base,
  public std::ofstream {

  bool committed;

public:
  atomic_ofstream(std::string orig_fn_):
    atomic_ofstream_base(orig_fn_),
    std::ofstream(tmp_fn.c_str()),
    committed(false)
  {}

  void commit() {
    if (!committed) {
      std::ofstream::close();
      // We don't need to remove the old file; rename() does this.
      int r = ::rename(tmp_fn.c_str(),orig_fn.c_str());
      if (r==-1) {
        throw_ErrnoException("rename()");
      }
      committed = true;
    }
  }

  ~atomic_ofstream() {
    if (!committed) {
      std::ofstream::close();
      ::unlink(tmp_fn.c_str());
    }
  }

};

};


#else
// fsync() implementation, broken on iPhone OS.


#include "Exception.hh"
#include "FileDescriptor.hh"

#ifndef LIBPBE_HAS_FILEDESCRIPTOR_STREAMS
#error FileDescriptor streams are needed for atomic_ofstream
#endif

#include <unistd.h>
#include <stdio.h>

#include <string>


namespace pbe {


struct atomic_ofstream_base {
  const std::string orig_fn;
  const std::string tmp_fn;
  FileDescriptor fd;
  atomic_ofstream_base(std::string orig_fn_):
    orig_fn(orig_fn_),
    tmp_fn(orig_fn+".tmp"),
    fd(tmp_fn,FileDescriptor::create)
  {
    fd.truncate(0);
  }
};


class atomic_ofstream:
  private atomic_ofstream_base,
  public FileDescriptor::ostream {

  bool committed;

public:
  atomic_ofstream(std::string orig_fn_):
    atomic_ofstream_base(orig_fn_),
    FileDescriptor::ostream(fd),
    committed(false)
  {}

  void commit() {
    if (!committed) {
      FileDescriptor::ostream::flush();
      fd.sync();
      fd.close();
      // We don't need to remove the old file; rename() does this.
      int r = ::rename(tmp_fn.c_str(),orig_fn.c_str());
      if (r==-1) {
        throw_ErrnoException("rename()");
      }
      committed = true;
    }
  }

  ~atomic_ofstream() {
    if (!committed) {
      ::unlink(tmp_fn.c_str());
    }
  }

};


};

#endif

#endif


