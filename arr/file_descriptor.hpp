#ifndef WRAP_FILE_DESCRIPTOR_HPP
#define WRAP_FILE_DESCRIPTOR_HPP
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

namespace wrap {

///
/// \ingroup system_resources
/// A file descriptor
///
struct file_descriptor {
  using fd_t = int;
  enum class fd_et : fd_t { invalid = -1 };
  ~file_descriptor() noexcept;
  file_descriptor() noexcept { }
  file_descriptor(fd_t descriptor) noexcept : fd(fd_et(descriptor)) { }
  file_descriptor(const file_descriptor& ) = delete;
  file_descriptor(      file_descriptor&&) noexcept;
  file_descriptor& operator=(const file_descriptor& ) = delete;
  file_descriptor& operator=(      file_descriptor&&) noexcept;
  bool valid() const noexcept { return fd != fd_et::invalid; }
  fd_t get()   const noexcept { return static_cast<fd_t>(fd); }
  fd_t release() noexcept { auto r = get(); fd = fd_et::invalid; return r; }
  void close();
private:
  fd_et fd = fd_et::invalid;
};

inline file_descriptor::file_descriptor(file_descriptor&& peer) noexcept
  : fd(std::move(peer.fd))
{
  peer.fd = fd_et::invalid;
}

inline file_descriptor&
file_descriptor::operator=(file_descriptor&& peer) noexcept {
  auto& self = *this;
  using std::swap;
  swap(self.fd, peer.fd);
  return self;
}

}

#endif
