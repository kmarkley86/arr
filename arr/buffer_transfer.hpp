#ifndef ARR_BUFFER_TRANSFER_HPP
#define ARR_BUFFER_TRANSFER_HPP
//
// Copyright (c) 2013, 2021, 2023
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

#include "arr/buffer_direction.hpp"
#include <type_traits>
#include <iterator>

namespace arr {

///
/// \ingroup buffers
/// Tracking data for one contiguous transfer involving a buffer
///
template <typename B>
struct buffer_transfer {
  using buffer_base = B;
  using value_type = typename buffer_base::value_type;
  using size_type  = typename buffer_base::size_type;
  using pointer    = typename buffer_base::pointer;
  using allocator_traits = typename buffer_base::allocator_traits;

  buffer_transfer(
      buffer_base& base,
      buffer_direction<size_type>& direction,
      wake_policy policy) noexcept
    : _base(base)
    , _direction(direction)
    , _current(_base.elements + _direction.offset())
    , _begin(_current)
    , _policy(policy)
  { }

  ~buffer_transfer() {
    _direction.increase_weak(
        B::as_size(_current - _begin),
        _base.capacity(),
        _policy);
  }

  template <typename S = size_type, typename V = value_type>
  typename std::enable_if<    std::is_trivially_destructible<V>::value,
  void>::type discard(S num) {
    _current += num;
  }

  template <typename output_iterator, typename V = value_type>
  typename std::enable_if<    std::is_trivially_copyable<V>::value,
  output_iterator>::type read(output_iterator dst, size_type num) {
    auto end = std::move(_current, _current + num, dst);
    _current += num;
    return end;
  }

  template <typename input_iterator, typename V = value_type>
  typename std::enable_if<    std::is_trivially_copyable<V>::value
    and std::is_base_of<std::forward_iterator_tag, typename
        std::iterator_traits<input_iterator>::iterator_category>::value,
  input_iterator>::type write(input_iterator src, size_type num) {
    auto end = std::next(src, B::as_diff(num));
    std::copy(src, end, _current);
    _current += num;
    return end;
  }


  template <typename S = size_type, typename V = value_type>
  typename std::enable_if<not std::is_trivially_destructible<V>::value,
  void>::type discard(S num) {
    while (num--) {
      allocator_traits::destroy(_base.allocator, _current);
      ++_current;
    }
  }

  template <typename output_iterator, typename V = value_type>
  typename std::enable_if<not std::is_trivially_copyable<V>::value,
  output_iterator>::type read(output_iterator dst, size_type num) {
    while (num--) {
      *dst = std::move(*_current);
      allocator_traits::destroy(_base.allocator, _current);
      ++dst;
      ++_current;
    }
    return dst;
  }

  template <typename input_iterator, typename V = value_type>
  typename std::enable_if<not std::is_trivially_copyable<V>::value
    or not std::is_base_of<std::forward_iterator_tag, typename
           std::iterator_traits<input_iterator>::iterator_category>::value,
  input_iterator>::type write(input_iterator src, size_type num) {
    while (num--) {
      allocator_traits::construct(_base.allocator, _current, *src);
      ++src;
      ++_current;
    }
    return src;
  }


  buffer_base&                 _base;
  buffer_direction<size_type>& _direction;
        pointer                _current;
  const pointer                _begin;
  wake_policy                  _policy;
};

}

#endif
