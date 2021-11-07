#ifndef ARR_RECURSIVE_DIRECTORY_SEQUENCE_HPP
#define ARR_RECURSIVE_DIRECTORY_SEQUENCE_HPP
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
#include <utility>
#include <deque>

namespace arr {

struct recursive_directory_sequence;

/// \addtogroup directory_traversal
/// @{

///
/// Iterator for the sequence of files in a directory tree
///
struct recursive_directory_iterator
  : std::iterator<std::input_iterator_tag, const struct dirent> {
  recursive_directory_iterator() noexcept { }
  recursive_directory_iterator(recursive_directory_sequence * rds);
  friend bool operator==(
      const recursive_directory_iterator& a,
      const recursive_directory_iterator& b) {
    return a.entry == b.entry;
  }
  friend bool operator!=(
      const recursive_directory_iterator& a,
      const recursive_directory_iterator& b) {
    return !(a==b);
  }
  recursive_directory_iterator& operator++();
  reference operator* () noexcept { return *entry; }
  pointer   operator->() noexcept { return  entry; }
private:
  recursive_directory_sequence * container = nullptr;
  pointer entry = nullptr;
  void filter();
  void descend();
  void descend_while_directory();
  void ascend_while_complete();
  void update_entry();
};

///
/// Filesystem recursive directory sequence with an iterator interface
///
struct recursive_directory_sequence {
  enum class dir_order {
    pre,        ///< Visit directory entry before contents of the directory
    post,       ///< Visit directory entry after contents of the directory
  };
  enum class visit_type {
    all,        ///< Visit all entries
    directory,  ///< Visit only directories
    file,       ///< Visit only regular files
    link,       ///< Visit only symbolic links
    non_dir,    ///< Visit only non-directories
  };
  recursive_directory_sequence(
      std::string directory,
      dir_order desired_order = dir_order::pre,
      visit_type desired_visit = visit_type::all
      )
    : _root(std::move(directory))
    , order(desired_order)
    , visit(desired_visit)
  { }
  recursive_directory_iterator begin();
  recursive_directory_iterator end() const noexcept {
    return recursive_directory_iterator();
  }
  const std::string& root() const { return _root; }
  const std::string& path() const { return context.back().first.path(); }

  ///
  /// Abandon the currently-processing directory
  ///
  /// When the iterator currently refers to a directory and the object was
  /// configured for \c dir_order::pre, that directory shall not be descended
  /// into.  In all other cases, the current directory is abandoned, and the
  /// next iterator increment shall not refer to a file in the current
  /// directory.  (For \c dir_order::post, it will refer to the directory
  /// that was just abandoned.)
  ///
  void abandon() noexcept { abandon_request = true; }

  ///
  /// Exceptions encountered during directory traversal
  ///
  std::list<std::unique_ptr<syscall_exception>> exceptions;
private:
  friend recursive_directory_iterator;
  std::deque<std::pair<directory_sequence, directory_iterator>> context;
  std::string _root;
  const dir_order order;
  const visit_type visit;
  bool abandon_request = false;
  void ascend();
  void descend(std::string subdir);
};

/// @}

}

#endif
