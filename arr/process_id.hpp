#ifndef ARR_PROCESS_ID_HPP
#define ARR_PROCESS_ID_HPP
//
// Copyright (c) 2012, 2021
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

#include <utility>
#include <string>
#include <sys/types.h>

namespace wrap {
///
/// \ingroup system_resources
/// A process result
///
struct process_result {
  using status_t = int;
  status_t status = -1;

  bool exited()    const noexcept;       ///< Process exited normally
  bool signaled()  const noexcept;       ///< Process terminated by signal
  bool coredump()  const noexcept;       ///< Process created core dump
  status_t exit_status() const noexcept; ///< Exit status of the process
  status_t term_signal() const noexcept; ///< Signal terminating the process
  bool successful() const noexcept;      ///< Process exited successfully
  std::string result() const;            ///< Description of process exit
};

///
/// \ingroup system_resources
/// A process id
///
struct process_id : process_result {
  enum class pid_et : pid_t { invalid = 0 };
  ~process_id() noexcept;       ///< Wait for the process to complete
  process_id() noexcept { }
  process_id(pid_t identifier) noexcept : pid(pid_et(identifier)) { }
  process_id(const process_id& ) = delete;
  process_id(      process_id&&) noexcept;
  process_id& operator=(const process_id& ) = delete;
  process_id& operator=(      process_id&&) noexcept;
  bool valid() const noexcept { return pid != pid_et::invalid; }
  pid_t get()  const noexcept { return static_cast<pid_t>(pid); }
  pid_t release() noexcept { auto r = get(); pid = pid_et::invalid; return r; }
  void wait();      ///< Wait for the process to complete
  bool completed(); ///< Check whether the process has completed

private:
  pid_et pid = pid_et::invalid;
  bool finished = false;
};

inline process_id::process_id(process_id&& peer) noexcept
  : process_result(peer)
  , pid(peer.pid)
  , finished(peer.finished)
{
  peer.pid = pid_et::invalid;
}

inline process_id&
process_id::operator=(process_id&& peer) noexcept {
  auto& self = *this;
  using std::swap;
  swap(static_cast<process_result&>(*this), static_cast<process_result&>(peer));
  swap(self.pid, peer.pid);
  swap(self.finished, peer.finished);
  return self;
}

}

#endif
