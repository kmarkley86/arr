#ifndef ARR_BUFFER_BASE_HPP
#define ARR_BUFFER_BASE_HPP
//
// Copyright (c) 2013, 2021
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

#include <memory>

namespace arr {

///
/// \ingroup buffers
/// A buffer allocated through an allocator
///
template <typename T, typename A = std::allocator<T>>
struct buffer_base {
  using value_type = T;
  using allocator_type = A;
  using allocator_traits = std::allocator_traits<A>;
  using size_type = typename allocator_traits::size_type;
  using pointer   = typename allocator_traits::pointer;

  using diff_type = decltype(std::declval<pointer>()-std::declval<pointer>());
  static auto as_size(diff_type n) { return static_cast<size_type>(n); }
  static auto as_diff(size_type n) { return static_cast<diff_type>(n); }

  ///
  /// Construct a buffer_base
  ///
  /// @param count The size of the buffer
  /// @param alloc Allocator to use for this buffer
  ///
  explicit buffer_base(
      size_type count,
      const allocator_type& alloc = allocator_type())
    : _capacity(count)
    , allocator(alloc)
    , elements(allocator_traits::allocate(allocator, capacity()))
  { }

  ~buffer_base() {
    allocator_traits::deallocate(allocator, elements, capacity());
  }

  buffer_base(const buffer_base& ) = delete;
  buffer_base(      buffer_base&&) = delete;
  buffer_base& operator=(const buffer_base& ) = delete;
  buffer_base& operator=(      buffer_base&&) = delete;

  size_type      _capacity;
  allocator_type allocator;
  pointer        elements;

  /// Returns the associated allocator
  allocator_type get_allocator() const { return allocator; }

  ///
  /// Returns the number of elements that can be held in currently allocated
  /// storage
  ///
  size_type capacity() const noexcept { return _capacity; }

  /// Returns the maximum possible number of elements
  size_type max_size() const noexcept {
    return allocator_traits::max_size(allocator);
  }

};

}

#endif
