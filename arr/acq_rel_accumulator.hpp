#ifndef ARR_ACQ_REL_ACCUMULATOR_HPP
#define ARR_ACQ_REL_ACCUMULATOR_HPP
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

#include <atomic>

namespace arr {

///
/// \ingroup buffers
/// An accumulator with acquire/release memory ordering semantics
///
template <typename T>
struct acq_rel_accumulator {
  using size_type = T;

  constexpr acq_rel_accumulator() noexcept : accumulator(0) { }

  /// Value of the accumulator
  size_type value() const noexcept {
    return accumulator.load(std::memory_order_acquire);
  }

  ///
  /// Increase the accumulator, as the only thread doing so
  ///
  /// @param amount Amount to increase the accumulator
  ///
  /// When only one thread updates the accumulator, it does not need
  /// to perform an atomic fetch_add.
  ///
  void increase_weak(size_type amount) noexcept {
    accumulator.store(value() + amount, std::memory_order_release);
  }

  ///
  /// Increase the accumulator, when several threads may do so
  ///
  /// @param amount Amount to increase the accumulator
  ///
  /// When several threads may be updating the accumulator, an atomic
  /// fetch_add is needed.  However, this implies stronger ordering
  /// than we need on some architectures.
  ///
  void increase_strong(size_type amount) noexcept {
    accumulator.fetch_add(amount, std::memory_order_acq_rel);
  }

private:
  std::atomic<size_type> accumulator;
};

}

#endif
