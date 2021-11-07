#ifndef ARR_DIRECTORY_SEQUENCE_HPP
#define ARR_DIRECTORY_SEQUENCE_HPP
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

#include "arr/directory.hpp"
#include "arr/syscall_exception.hpp"
#include <iterator>
#include <string>
#include <list>
#include <memory>

namespace arr {

struct directory_sequence;

/// \addtogroup directory_traversal
/// @{

///
/// Iterator for the sequence of files in a directory
///
struct directory_iterator
  : std::iterator<std::input_iterator_tag, const struct dirent> {
  directory_iterator() noexcept { }
  directory_iterator(directory_sequence * ds);
  friend bool operator==(
      const directory_iterator& a,
      const directory_iterator& b) {
    return a.entry == b.entry;
  }
  friend bool operator!=(
      const directory_iterator& a,
      const directory_iterator& b) {
    return !(a==b);
  }
  directory_iterator& operator++();
  reference operator* () noexcept { return *entry; }
  pointer   operator->() noexcept { return  entry; }
private:
  wrap::directory dir;
  directory_sequence * container = nullptr;
  pointer entry = nullptr;
};

///
/// Filesystem directory sequence with an iterator interface
///
struct directory_sequence {
  directory_sequence(std::string directory);
  directory_iterator begin();
  directory_iterator end()  const noexcept { return directory_iterator(); }
  const std::string& path() const noexcept { return _path; }

  ///
  /// Exceptions encountered during directory traversal
  ///
  std::list<std::unique_ptr<syscall_exception>> exceptions;
private:
  std::string _path;
};

/// @}

}

#endif
