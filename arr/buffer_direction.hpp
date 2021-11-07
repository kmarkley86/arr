#ifndef ARR_BUFFER_DIRECTION_HPP
#define ARR_BUFFER_DIRECTION_HPP
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

#include "arr/recent_accumulator.hpp"

namespace arr {

///
/// \ingroup buffers
/// Tracking data for one direction of a buffer
///
template <typename T>
struct buffer_direction : private recent_accumulator<T> {
  using size_type = T;

  constexpr buffer_direction() noexcept : _offset(0) { }

  using recent_accumulator<T>::total;
  using recent_accumulator<T>::recent;
  using recent_accumulator<T>::reset_recent;

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
  void increase_weak(size_type num, size_type wrap) noexcept {
    increase_common(num, wrap);
    recent_accumulator<T>::increase_weak(num);
  }

  ///
  /// Increase the number of elements, when several threads may do so
  ///
  /// @param num  Number of additional elements
  /// @param wrap Offset at which the buffer wraps
  ///
  /// \c num must not be greater than \c wrap.
  ///
  void increase_strong(size_type num, size_type wrap) noexcept {
    increase_common(num, wrap);
    recent_accumulator<T>::increase_strong(num);
  }

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

  size_type       _offset;    ///< Offset within the buffer
};

}

#endif
