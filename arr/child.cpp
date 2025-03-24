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

#include "arr/child.hpp"
#include "arr/path_exception.hpp"
#include "arr/fcntl.hpp"
#include <algorithm>
#include <iostream>

namespace {

struct _exit_sentinel {
  [[noreturn]] ~_exit_sentinel() { wrap::_exit(SOURCE_CONTEXT, -1); }
};

void dup_or_close(int oldd, int newd) {
  if (-1 != oldd) {
    wrap::dup2(SOURCE_CONTEXT, oldd, newd);
  } else {
    wrap::close(SOURCE_CONTEXT, newd);
  }
}

[[noreturn]] void child_stdio(
    const char * const argv[],
    const char * const envp[],
    int fd_stdin, int fd_stdout, int fd_stderr) {
  _exit_sentinel s;
  try {
    dup_or_close(fd_stdin , STDIN_FILENO );
    dup_or_close(fd_stdout, STDOUT_FILENO);
    dup_or_close(fd_stderr, STDERR_FILENO);
    try {
      wrap::closefrom(SOURCE_CONTEXT,
          1+std::max({STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO}));
    } catch (arr::errno_exception& e) {
      if (e.code() == std::errc::bad_file_descriptor) {
        // Swallow EBADF, because there might legitimately be no other
        // file descriptors to close.
      } else {
        throw;
      }
    }
    wrap::execve(SOURCE_CONTEXT, argv[0], argv, envp);
  } catch (arr::context_exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

}

namespace arr {

pid_t spawn_stdio(
    const char * const argv[],
    const char * const envp[],
    int fd_stdin, int fd_stdout, int fd_stderr) {
  const char * path = argv[0];
  wrap::access(SOURCE_CONTEXT, path, X_OK);
  struct stat sb;
  wrap::stat(SOURCE_CONTEXT, path, &sb);
  if (S_ISREG(sb.st_mode)) {
    // Should be executable; okay to proceed
  } else if (S_ISDIR(sb.st_mode)) {
    auto e = std::make_error_code(std::errc::is_a_directory);
    throw arr::path_exception(SOURCE_CONTEXT, "execve", path, e.value());
  } else {
    // OpenBSD gives EACCES when trying to execute a FIFO, even if it has
    // execute permissions.  I think ENOTSUP is a more meaningful error,
    // and this lets us handle the error directly in the parent process
    // instead of trying to figure out why the child process died.
    auto e = std::make_error_code(std::errc::not_supported);
    throw arr::path_exception(SOURCE_CONTEXT, "execve", path, e.value());
  }
  pid_t pid = wrap::fork(SOURCE_CONTEXT);
  if (0 == pid) {
    child_stdio(argv, envp, fd_stdin, fd_stdout, fd_stderr);
  }
  return pid;
}

pid_t spawn_stdio(
    const arguments& argv,
    const char * const envp[],
    int fd_stdin, int fd_stdout, int fd_stderr) {
  return spawn_stdio(argv.as_argv().get(), envp,
      fd_stdin, fd_stdout, fd_stderr);
}

}
