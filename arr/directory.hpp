#ifndef WRAP_DIRECTORY_HPP
#define WRAP_DIRECTORY_HPP
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

#include <sys/types.h>
#include <dirent.h>
#include <utility>

namespace wrap {

///
/// \ingroup system_resources
/// A directory pointer
///
struct directory {
  using dir_t = DIR *;
  ~directory() noexcept;
  directory() noexcept { }
  directory(dir_t dirp) noexcept : dir(dirp) { }
  directory(const directory& ) = delete;
  directory(      directory&&) noexcept;
  directory& operator=(const directory& ) = delete;
  directory& operator=(      directory&&) noexcept;
  bool valid() const noexcept { return nullptr != dir; }
  dir_t get()  const noexcept { return dir; }
  dir_t release() noexcept { auto r = get(); dir = nullptr; return r; }
  void close();
private:
  dir_t dir = nullptr;
};

inline directory::directory(directory&& peer) noexcept
  : dir(std::move(peer.dir))
{
  peer.dir = nullptr;
}

inline directory&
directory::operator=(directory&& peer) noexcept {
  auto& self = *this;
  using std::swap;
  swap(self.dir, peer.dir);
  return self;
}

}

#endif
