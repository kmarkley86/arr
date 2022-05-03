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

#include "arr/directory_sequence.hpp"
#include "arr/dirent.hpp"
#include <utility>

namespace {
  const std::string current_dir(".");
  const std::string parent_dir("..");
  constexpr char separator = '/';
}

namespace arr {

directory_iterator::directory_iterator(directory_sequence * ds)
  : dir( ds ? wrap::opendir(SOURCE_CONTEXT, ds->path().c_str()) : nullptr )
  , container(ds)
{
  ++*this;
}

directory_iterator& directory_iterator::operator++() {
  if (dir.valid()) {
    try {
      do {
        entry = wrap::readdir(SOURCE_CONTEXT, dir.get());
      } while (entry and DT_DIR == entry->d_type and
          (current_dir == entry->d_name or
            parent_dir == entry->d_name));
    } catch (syscall_exception& e) {
      entry = nullptr;
      container->exceptions.emplace_back(e.clone());
    }
  }
  return *this;
}

directory_sequence::directory_sequence(std::string dir)
  : _path(std::move(dir))
{
  if (_path.empty() or separator != _path.back()) {
    _path += separator;
  }
}

directory_iterator directory_sequence::begin() {
  try {
    return directory_iterator(this);
  } catch (syscall_exception& e) {
    exceptions.emplace_back(e.clone());
    return end();
  }
}

}
