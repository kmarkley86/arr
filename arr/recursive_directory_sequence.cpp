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

#include "arr/recursive_directory_sequence.hpp"
#include <cstdlib>
#include <utility>
#include <tuple>

//
// Note that all exceptions related to system calls are caught by the
// underlying directory_sequence objects.  The recursive_directory_sequence
// merely needs to gather those exceptions; it does not need to catch any.
//

namespace {
  template <typename T> void steal(std::list<T>& dst, std::list<T>& src) {
    dst.splice(dst.end(), src);
  }
}

namespace arr {

recursive_directory_iterator::recursive_directory_iterator(
    recursive_directory_sequence * rds)
  : container(rds)
  , entry(nullptr)
{
  if (container) {
    container->descend(container->root());
    //
    // If using post-order for directories, descend into them.
    //
    if (recursive_directory_sequence::dir_order::post == container->order) {
      descend_while_directory();
    }
    ascend_while_complete();
    update_entry();
    filter();
  }
}

void recursive_directory_iterator::filter() {
  switch (container->visit) {
    case recursive_directory_sequence::visit_type::all:
      break;
    case recursive_directory_sequence::visit_type::directory:
      while (entry and DT_DIR != entry->d_type) operator++();
      break;
    case recursive_directory_sequence::visit_type::file:
      while (entry and DT_REG != entry->d_type) operator++();
      break;
    case recursive_directory_sequence::visit_type::link:
      while (entry and DT_LNK != entry->d_type) operator++();
      break;
    case recursive_directory_sequence::visit_type::non_dir:
      while (entry and DT_DIR == entry->d_type) operator++();
      break;
  }
}

void recursive_directory_iterator::descend() {
  auto& iter = container->context.back().second;
  container->descend(container->path() + iter->d_name);
}

void recursive_directory_iterator::descend_while_directory() {
  auto& context = container->context;
  const directory_iterator end;
  while (end != context.back().second and
      DT_DIR == context.back().second->d_type) {
    descend();
  }
}

void recursive_directory_iterator::ascend_while_complete() {
  auto& context = container->context;
  const directory_iterator end;
  while (not context.empty() and end == context.back().second) {
    container->ascend();
  }
}

void recursive_directory_iterator::update_entry() {
  if (container->context.empty()) {
    entry = nullptr;
  } else {
    entry = &*container->context.back().second;
  }
}

recursive_directory_iterator& recursive_directory_iterator::operator++() {
  //
  // If there is an abandon_request, we will abandon the current directory
  // unless the current entry is a directory and we are dir_order::pre.  In
  // that case we don't abandon anything -- we simply don't descend into it.
  //
  // If we abandon a directory, we should not increment the iterator, because
  // the act of abandoning moves us to the proper position in the sequence.
  //
  bool abandoned_directory = false;
  if (entry and container->abandon_request) {
    if (not (DT_DIR == entry->d_type and
          recursive_directory_sequence::dir_order::pre == container->order)) {
      container->ascend();
      ascend_while_complete();
      update_entry();
      abandoned_directory = true;
    }
  }
  if (entry) {
    auto& context = container->context;
    auto& iter    = context.back().second;
    switch (container->order) {
      case recursive_directory_sequence::dir_order::pre:
        if (DT_DIR == entry->d_type and not container->abandon_request) {
          descend();
        }
        if (not abandoned_directory) {
          ++iter;       // References the pre-descend() iterator
        }
        break;
      case recursive_directory_sequence::dir_order::post:
        if (not abandoned_directory) {
          ++iter;
          descend_while_directory();
        }
        break;
    }
    container->abandon_request = false;
    ascend_while_complete();
    update_entry();
    filter();
  }
  return *this;
}

recursive_directory_iterator recursive_directory_sequence::begin() {
  return recursive_directory_iterator(this);
}

void recursive_directory_sequence::ascend() {
  steal(exceptions, context.back().first.exceptions);
  context.pop_back();
}

void recursive_directory_sequence::descend(std::string subdir) {
  if (not context.empty()) steal(exceptions, context.back().first.exceptions);
  context.emplace_back(
      std::piecewise_construct,
      std::forward_as_tuple(std::move(subdir)),
      std::forward_as_tuple());
  context.back().second = context.back().first.begin();
}

}
