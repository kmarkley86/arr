#ifndef ARR_CHILD_HPP
#define ARR_CHILD_HPP
//
// Copyright (c) 2012, 2021, 2025
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

#include "arr/unistd.hpp"
#include "arr/pipe.hpp"
#include "arr/arg_env.hpp"

namespace arr {

/// \addtogroup child_processes
/// @{

///
/// Spawn a child process, controlling its standard I/O
///
/// @param argv Arguments - argv[0] specifies the new process file
/// @param envp Environment (defaults to the current environment)
/// @param fd_stdin  File descriptor to become the child's stdin
/// @param fd_stdout File descriptor to become the child's stdout
/// @param fd_stderr File descriptor to become the child's stderr
/// @return The process ID of the child process
///
/// The file descriptors are optional arguments; the default is to re-use
/// the current stdin, stdout, and stderr.  If the value -1 is passed for
/// any file descriptor, that stream will be disconnected in the child.
///
/// Throws a path_exception if \c argv[0] is not executable, or if
/// the child process could not be created.  Some errors are only detectable
/// in the child process and will cause it to exit with a -1 status code.
///
pid_t spawn_stdio(
    const char * const argv[],
    const char * const envp[] = ::environ,
    int fd_stdin  = STDIN_FILENO,
    int fd_stdout = STDOUT_FILENO,
    int fd_stderr = STDERR_FILENO);
pid_t spawn_stdio(
    const arguments& argv,
    const char * const envp[] = ::environ,
    int fd_stdin  = STDIN_FILENO,
    int fd_stdout = STDOUT_FILENO,
    int fd_stderr = STDERR_FILENO);

///
/// Communication channel with parent and child sides
///
struct channel {
  virtual ~channel() = default;
  virtual wrap::file_descriptor& parent() noexcept = 0;
  virtual wrap::file_descriptor&  child() noexcept = 0;
  void close_parent() { auto& p = parent(); if (p.valid()) p.close(); }
  void close_child()  { auto& c =  child(); if (c.valid()) c.close(); }
};

///
/// Close the file descriptor in the child process
///
struct no_channel : public channel, private wrap::file_descriptor {
  virtual ~no_channel() = default;
  virtual wrap::file_descriptor& parent() noexcept { return *this; }
  virtual wrap::file_descriptor&  child() noexcept { return *this; }
};

///
/// File descriptor to give to a child process
///
struct fd_channel : public channel, private wrap::file_descriptor {
  fd_channel(wrap::file_descriptor& descriptor)
    : wrap::file_descriptor(std::move(descriptor)) { }
  fd_channel(wrap::file_descriptor::fd_t descriptor)
    : wrap::file_descriptor(descriptor) { }
  wrap::file_descriptor none;
  virtual ~fd_channel() = default;
  virtual wrap::file_descriptor& parent() noexcept { return  none; }
  virtual wrap::file_descriptor&  child() noexcept { return *this; }
};

///
/// Pipe for reading from a child process
///
struct  read_pipe : public channel, private arr::pipe {
  read_pipe() : arr::pipe(SOURCE_CONTEXT) { }
  virtual ~read_pipe() = default;
  virtual wrap::file_descriptor& parent() noexcept { return  read; }
  virtual wrap::file_descriptor&  child() noexcept { return write; }
};

///
/// Pipe for writing to a child process
///
struct write_pipe : public channel, private arr::pipe {
  write_pipe() : arr::pipe(SOURCE_CONTEXT) { }
  virtual ~write_pipe() = default;
  virtual wrap::file_descriptor& parent() noexcept { return write; }
  virtual wrap::file_descriptor&  child() noexcept { return  read; }
};

/// @}

}

#endif
