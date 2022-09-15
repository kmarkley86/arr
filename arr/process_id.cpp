//
// Copyright (c) 2012, 2021, 2022
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
#include "arr/wait.hpp"
#include "arr/syscall_exception.hpp"
#include <cstdlib>
#include <stdexcept>
#include <sstream>

namespace wrap {

void process_id::wait() {
  while (not finished) {
    try {
      finished = wrap::waitpid(SOURCE_CONTEXT, get(), &status, 0);
    } catch (arr::syscall_exception& e) {
      if (e.code() != std::errc::interrupted) throw;
    }
  }
}

bool process_id::completed() {
  if (not finished) {
    finished = wrap::waitpid(SOURCE_CONTEXT, get(), &status, WNOHANG);
  }
  return finished;
}

bool process_result::exited() const noexcept {
  return WIFEXITED(status);
}

bool process_result::signaled() const noexcept {
  return WIFSIGNALED(status);
}

bool process_result::coredump() const noexcept {
  return signaled() and WCOREDUMP(status);
}

process_result::status_t process_result::exit_status() const noexcept {
  return WEXITSTATUS(status);
}

process_result::status_t process_result::term_signal() const noexcept {
  return WTERMSIG(status);
}

bool process_result::successful() const noexcept {
  return exited() and EXIT_SUCCESS == exit_status();
}

// TODO I18N
std::string process_result::result() const {
  std::ostringstream s;
  if (false) {
  } else if (exited()) {
    s << "exited with status " << exit_status();
  } else if (signaled()) {
    s << "terminated by signal " << term_signal();
    if (coredump()) {
      s << ", core dumped";
    }
  } else {
    throw std::runtime_error("process neither exited nor signaled");
  }
  return s.str();
}

process_id::~process_id() noexcept {
  if (valid() and not finished) wait();
}

}
