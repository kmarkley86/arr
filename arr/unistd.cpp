//
// Copyright (c) 2012, 2014, 2021
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
#include "arr/path_exception.hpp"
#include <thread>

#ifdef __OpenBSD__
#define HAS_CLOSEFROM
#endif

namespace wrap {

void execve(
    arr::source_context context,
    const char * path,
    const char * const argv[],
    const char * const envp[]) {
  //
  // Cast away some const-ness for the actual call
  //
  auto arg = const_cast<char * const *>(argv);
  auto env = const_cast<char * const *>(envp);
  if (argv) {
    ::execve(path, arg, env);
  } else {
    auto pth = const_cast<char *>(path);
    char * const args[] = { pth, nullptr };
    ::execve(path, args, env);
  }
  throw arr::path_exception(context, __func__, path);
}

void close(arr::source_context context, int d) {
  auto r = ::close(d);
  if (0 != r) throw arr::syscall_exception(context, __func__);
}

off_t lseek(arr::source_context context,
    int fildes, off_t offset, int whence) {
  auto r = ::lseek(fildes, offset, whence);
  if (-1 == r) throw arr::syscall_exception(context, __func__);
  return r;
}

void dup2(arr::source_context context, int oldd, int newd) {
  auto r = ::dup2(oldd, newd);
  if (-1 == r) throw arr::syscall_exception(context, __func__);
}

#ifdef HAS_CLOSEFROM
void closefrom(arr::source_context context, int fd) {
  auto r = ::closefrom(fd);
  if (0 != r) throw arr::syscall_exception(context, __func__);
}
#else
void closefrom(arr::source_context, int fd) {
  auto max = ::getdtablesize();
  while (fd < max) ::close(fd++);
  errno = 0;
}
#endif

void pipe(arr::source_context context, int fdes[2]) {
  auto r = ::pipe(fdes);
  if (0 != r) throw arr::syscall_exception(context, __func__);
}

void _exit(arr::source_context, int status) {
  ::_exit(status);
}

pid_t fork(arr::source_context context) {
  auto r = ::fork();
  if (-1 == r) {
    if (EAGAIN == errno and std::chrono::microseconds(0) != fork_retry_delay) {
      std::this_thread::sleep_for(fork_retry_delay);
      return fork(context);
    } else {
      throw arr::syscall_exception(context, __func__);
    }
  }
  return r;
}

std::chrono::microseconds fork_retry_delay(0);

void access(arr::source_context context, const char * path, int amode) {
  auto r = ::access(path, amode);
  if (0 != r) throw arr::path_exception(context, __func__, path);
}

size_t write(arr::source_context context, int d, const void *buf, size_t nbytes) {
  auto r = ::write(d, buf, nbytes);
  if (-1 == r) throw arr::syscall_exception(context, __func__);
  return size_t(r);
}

void unlink(arr::source_context context, const char * path) {
  auto r = ::unlink(path);
  if (0 != r) throw arr::path_exception(context, __func__, path);
}

void rmdir(arr::source_context context, const char * path) {
  auto r = ::rmdir(path);
  if (0 != r) throw arr::path_exception(context, __func__, path);
}

void chdir(arr::source_context context, const char * path) {
  auto r = ::chdir(path);
  if (0 != r) throw arr::path_exception(context, __func__, path);
}

}
