//
// Copyright (c) 2012, 2013, 2021
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

#include "arr/execute.hpp"
#include "arr/fcntl.hpp"

namespace arr {

execute::execute(const char * const argv[])
  : wrap::process_id(arr::spawn_stdio(argv))
{
}

execute_capture::execute_capture(
    const char * const argv[], bool capture_stderr)
  : arr::read_pipe()
  , wrap::process_id(arr::spawn_stdio(argv, ::environ,
        STDIN_FILENO,
        child().get(),
        capture_stderr ? child().get() : STDERR_FILENO))
  , arr::fd_istream(parent())
{
  close_child();
}

execute_log_fd::execute_log_fd(
    const char * const argv[], 
    wrap::file_descriptor::fd_t descriptor,
    bool log_stderr)
  : wrap::process_id(arr::spawn_stdio(argv, ::environ,
        STDIN_FILENO,
        descriptor,
        log_stderr ? descriptor : STDERR_FILENO))
{
}

execute_log_file::execute_log_file(
    const char * const argv[], 
    const char * path, int flags, mode_t mode,
    bool log_stderr)
  : arr::fd_channel(wrap::open(SOURCE_CONTEXT, path, flags, mode))
  , execute_log_fd(argv, child().get(), log_stderr)
{
  close_child();
}

execute_io::execute_io(
    const char * const argv[], bool capture_stderr)
  : arr::write_pipe()
  , arr::read_pipe()
  , wrap::process_id(arr::spawn_stdio(argv, ::environ,
        arr::write_pipe::child().get(),
        arr::read_pipe::child().get(),
        capture_stderr ? arr::read_pipe::child().get() : STDERR_FILENO))
  , read(arr::read_pipe::parent())
  , write(arr::write_pipe::parent())
{
  arr::read_pipe::close_child();
  arr::write_pipe::close_child();
}

}
