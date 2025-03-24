#ifndef ARR_EXECUTE_HPP
#define ARR_EXECUTE_HPP
//
// Copyright (c) 2012, 2013, 2021, 2025
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

#include "arr/process_id.hpp"
#include "arr/child.hpp"
#include "arr/fd_stream.hpp"
#include "arr/arg_env.hpp"
#include <utility>

namespace arr {

/// \addtogroup child_processes
/// @{

///
/// Start a child process
///
/// All standard I/O is shared between parent and child.
///
struct execute
  : public wrap::process_id
{
  execute(const char * const argv[]);
  execute(const arguments& argv);
};

///
/// Start a child process, capturing its output
///
/// Standard input is shared between parent and child.
/// Standard output is captured.
/// Standard error may be captured in the same stream.
///
struct execute_capture
  : private arr::read_pipe
  , public wrap::process_id
  , public arr::fd_istream
{
  execute_capture(const char * const argv[], bool capture_stderr = false);
  execute_capture(const arguments& argv, bool capture_stderr = false);
};

///
/// Start a child process, logging its output to a file descriptor
///
/// Standard input is shared between parent and child.
/// Standard output is directed to a file descriptor.
/// Standard error may be directed to the same file descriptor.
///
struct execute_log_fd
  : public wrap::process_id
{
  execute_log_fd(
      const char * const argv[],
      wrap::file_descriptor::fd_t descriptor,
      bool log_stderr = false);
  execute_log_fd(
      const arguments& argv,
      wrap::file_descriptor::fd_t descriptor,
      bool log_stderr = false);
};

///
/// Start a child process, logging its output to a file
///
/// Standard input is shared between parent and child.
/// Standard output is directed to a file.
/// Standard error may be directed to the same file.
///
struct execute_log_file
  : private arr::fd_channel
  , public execute_log_fd
{
  execute_log_file(
      const char * const argv[],
      const char * path, int flags, mode_t mode,
      bool log_stderr = false);
  execute_log_file(
      const arguments& argv,
      const char * path, int flags, mode_t mode,
      bool log_stderr = false);
};

///
/// Start a child process with bidirectional communication
///
/// Establish bidirectional communication with the child process via its
/// standard input and output.  The child process's standard error may be
/// captured along with its standard output.
///
/// As a convenience, this object accepts formatted I/O, but for complex
/// usages the actual read and write streams are available as public members.
///
struct execute_io
  : private arr::write_pipe
  , private arr::read_pipe
  , public wrap::process_id
{
  execute_io(const char * const argv[], bool capture_stderr = false);
  execute_io(const arguments& argv, bool capture_stderr = false);
  //
  // Must use containment, not inheritance, due to virtual inheritance
  // from basic_ios in both istream and ostream.
  //
  arr::fd_istream read;         ///< Stream from child process
  arr::fd_ostream write;        ///< Stream to child process
  //
  // For convenience, we want to allow formatted I/O operations without
  // explicitly fetching the stream member.  Because stream manipulators are
  // template functions, they cannot be deduced and a separate overload is
  // needed.
  //
  template <typename T> arr::fd_istream& operator>>(T&& value) {
    read  >> std::forward<T>(value); return read;
  }
  template <typename T> arr::fd_ostream& operator<<(T&& value) {
    write << std::forward<T>(value); return write;
  }
  arr::fd_istream& operator>>(std::istream& (*f)(std::istream&)) {
    read  >> f; return read;
  }
  arr::fd_ostream& operator<<(std::ostream& (*f)(std::ostream&)) {
    write << f; return write;
  }
};

/// @}

}

#endif
