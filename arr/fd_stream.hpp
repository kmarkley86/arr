#ifndef ARR_FD_STREAM_HPP
#define ARR_FD_STREAM_HPP
//
// Copyright (c) 2012, 2018, 2021
// Kyle Markley.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the author nor the names of any contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "arr/file_descriptor.hpp"
#include <istream>
#include <ostream>
#ifdef _LIBCPP_VERSION
#include <cstdio>
#include <__std_stream>
#include "arr/syscall_exception.hpp"
#else
#include <ext/stdio_filebuf.h>
#endif

namespace arr {

#ifdef _LIBCPP_VERSION

constexpr const char * stdio_mode(std::ios_base::openmode mode) {
  if (mode & std::ios_base::in) {
    if (mode & std::ios_base::out) {
      return "r+";
    } else {
      return "r";
    }
  } else {
    if (mode & std::ios_base::out) {
      return "w";
    } else {
      return "";
    }
  }
}

struct FILEwrap {
  FILE * file;
  typename std::char_traits<char>::state_type state;
  int desc;
  FILEwrap(int descriptor, std::ios_base::openmode mode)
    : file(fdopen(descriptor, stdio_mode(mode)))
    , desc(descriptor)
  {
    if (not file) throw syscall_exception("fdopen");
  }
  ~FILEwrap() {
    if (file) std::fclose(file);
  }
  int fd() noexcept { return desc; }
};

struct ifilebuf : FILEwrap, std::__stdinbuf<char> {
  ifilebuf(int descriptor, std::ios_base::openmode mode)
    : FILEwrap(descriptor, mode)
    , std::__stdinbuf<char>(file, &state)
    { }
};

struct ofilebuf : FILEwrap, std::__stdoutbuf<char> {
  ofilebuf(int descriptor, std::ios_base::openmode mode)
    : FILEwrap(descriptor, mode)
    , std::__stdoutbuf<char>(file, &state)
    { }
};

# else

using ifilebuf = __gnu_cxx::stdio_filebuf<char>;
using ofilebuf = __gnu_cxx::stdio_filebuf<char>;

#endif

/// \addtogroup miscellaneous
/// @{

///
/// An istream referencing a file descriptor
///
/// The file descriptor becomes owned by the this object.
//
// The stdio_filebuf takes ownership of the descriptor, but the istream does
// not take ownership of the stdio_filebuf, so inheritance lets us manage
// all the resources correctly.
//
struct fd_istream
  : public ifilebuf
  , public std::istream
{

  fd_istream(wrap::file_descriptor& descriptor,
      std::ios_base::openmode mode = std::ios_base::in)
    : ifilebuf(descriptor.get(), mode)
    , std::istream((static_cast<void>(descriptor.release()), this))
  {
    // Note that the file_descriptor releases ownership after stdio_filebuf
    // construction but before istream construction, so that ownership is
    // exception-safe.
    //
    // There is no constructor taking an unmanaged file descriptor due to
    // the potential for an exception that would make it unclear whether
    // the descriptor had been closed.
  }

};

///
/// An ostream referencing a file descriptor
///
/// The file descriptor becomes owned by the stdio_filebuf.
//
// The stdio_filebuf takes ownership of the descriptor, but the ostream does
// not take ownership of the stdio_filebuf, so inheritance lets us manage
// all the resources correctly.
//
struct fd_ostream
  : public ofilebuf
  , public std::ostream
{

  fd_ostream(wrap::file_descriptor& descriptor,
      std::ios_base::openmode mode = std::ios_base::out)
    : ofilebuf(descriptor.get(), mode)
    , std::ostream((static_cast<void>(descriptor.release()), this))
  {
    // Note that the file_descriptor releases ownership after stdio_filebuf
    // construction but before ostream construction, so that ownership is
    // exception-safe.
    //
    // There is no constructor taking an unmanaged file descriptor due to
    // the potential for an exception that would make it unclear whether
    // the descriptor had been closed.
  }

};

/// @}

}

#endif
