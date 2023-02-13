#ifndef ARR_BUFFER_DIRECTION_HPP
#define ARR_BUFFER_DIRECTION_HPP
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

#include "arr/recent_accumulator.hpp"
#include <atomic>

namespace arr {

enum class wake_policy { one, all };

///
/// \ingroup buffers
/// Tracking data for one direction of a buffer
///
template <typename T>
struct buffer_direction : private recent_accumulator<T> {
  using base = recent_accumulator<T>;
  using size_type = T;
  using enum std::memory_order;

  constexpr buffer_direction() noexcept : _offset(0u) { }

  using base::total;
  using base::recent;
  using base::reset_recent;

  /// Buffer storage offset of next element
  size_type offset() const noexcept { return _offset; }

  ///
  /// Increase the number of elements, as the only thread doing so
  ///
  /// @param num  Number of additional elements
  /// @param wrap Offset at which the buffer wraps
  ///
  /// \c num must not be greater than \c wrap.
  ///
  void increase_weak(
      size_type num,
      size_type wrap,
      wake_policy policy) noexcept {
    increase_common(num, wrap);
    base::increase_weak(num);
    notify_common(policy);
  }

  ///
  /// Increase the number of elements, when several threads may do so
  ///
  /// @param num  Number of additional elements
  /// @param wrap Offset at which the buffer wraps
  ///
  /// \c num must not be greater than \c wrap.
  ///
  void increase_strong(
      size_type num,
      size_type wrap,
      wake_policy policy) noexcept {
    increase_common(num, wrap);
    base::increase_strong(num);
    notify_common(policy);
  }

  void wait(size_type old = total(),
      std::memory_order order = seq_cst) noexcept {
    _waiters.fetch_add(1u, acq_rel);
    base::wait(old, order);
    _waiters.fetch_sub(1u, acq_rel);
  }

  size_type waiters() const noexcept { return _waiters; }

private:

  ///
  /// Common implementation of increase
  ///
  /// These operations must be ordered before the increase in the accumulator,
  /// to ensure they are seen when another thread observes the accumulator.
  ///
  /// \c num must not be greater than \c wrap.
  ///
  void increase_common(size_type num, size_type wrap) noexcept {
    _offset += num;
    if (_offset >= wrap) _offset -= wrap;
  }

  void notify_common(wake_policy policy) noexcept {
    if (_waiters.load(acquire)) {
      switch (policy) {
        case wake_policy::one:
          base::notify_one();
          break;
        case wake_policy::all:
          base::notify_all();
          break;
      }
    }
  }

              size_type  _offset;  ///< Offset within the buffer
  std::atomic<size_type> _waiters; ///< Number of waiters
};

}

#endif
