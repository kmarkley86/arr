#ifndef WRAP_UNISTD_HPP
#define WRAP_UNISTD_HPP
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

#include "arr/source_context.hpp"
#include <chrono>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern "C" char **environ; ///< User environment

///
/// \file
/// \ingroup system_function_wrappers
///
/// Wrappers for functions in \c <unistd.h>
///

namespace wrap {

/// \addtogroup system_function_wrappers
/// @{

///
/// Wrapper for execve(2)
///
/// @param path New process file
/// @param argv Arguments (defaults to \c path as the only argument)
/// @param envp Environment (defaults to the current environment)
///
/// Replace the current process with a new process.
///
/// This throws a path_exception if the underlying function fails.
///
/// This interface is more const-correct than the standard.
/// http://stackoverflow.com/questions/190184/execv-and-const-ness
///
[[noreturn]] void execve(
    arr::source_context,
    const char * path,
    const char * const argv[] = nullptr,
    const char * const envp[] = ::environ);

///
/// Wrapper for close(2)
///
void close(arr::source_context, int d);

///
/// Wrapper for lseek(2)
///
off_t lseek(arr::source_context, int fildes, off_t offset, int whence);

///
/// Wrapper for dup2(2)
///
void dup2(arr::source_context, int oldd, int newd);

///
/// Wrapper for closefrom(2)
///
void closefrom(arr::source_context, int fd);

///
/// Wrapper for pipe(2)
///
void pipe(arr::source_context, int fildes[2]);

///
/// Wrapper for _exit(2)
///
[[noreturn]] void _exit(arr::source_context, int status);

///
/// Wrapper for fork(2)
///
pid_t fork(arr::source_context);

///
/// Delay this long and retry if fork(2) fails with EAGAIN
///
/// The default delay value, 0, throws instead of retrying.
///
extern std::chrono::microseconds fork_retry_delay;

///
/// Wrapper for access(2)
///
void access(arr::source_context, const char * path, int amode);

///
/// Wrapper for write(2)
///
size_t write(arr::source_context, int d, const void *buf, size_t nbytes);

///
/// Wrapper for unlink(2)
///
void unlink(arr::source_context, const char *path);

///
/// Wrapper for rmdir(2)
///
void rmdir(arr::source_context, const char *path);

///
/// Wrapper for chdir(2)
///
void chdir(arr::source_context, const char *path);

/// @}

}

#endif
