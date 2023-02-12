#ifndef ARR_RECENT_ACCUMULATOR_HPP
#define ARR_RECENT_ACCUMULATOR_HPP
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

#include <atomic>

namespace arr {

///
/// \ingroup buffers
/// Atomic accumulator with a short-term memory
///
template <typename T>
struct recent_accumulator {
  using size_type = T;
  using enum std::memory_order;

  constexpr recent_accumulator() noexcept : _recent(0u), _total(0u) { }

  /// Total value of the accumulator
  size_type total () const noexcept { return _total.load(acquire); }

  /// Recent addition to the accumulator
  size_type recent() const noexcept { return _recent; }

  /// Reset the counter of recent addition
  void reset_recent() noexcept { _recent = 0u; }

  ///
  /// Increase the accumulator, as the only thread doing so
  ///
  /// @param amount Amount to increase the accumulator
  ///
  /// When only one thread updates the accumulator, it does not need
  /// to perform an atomic fetch_add.
  ///
  void increase_weak(size_type amount) noexcept {
    increase_common(amount);
    _total.store(total() + amount, release);
  }

  ///
  /// Increase the accumulator, when several threads may do so
  ///
  /// @param amount Amount to increase the accumulator
  ///
  /// When several threads may be updating the accumulator, an atomic
  /// fetch_add is needed.  However, this implies stronger ordering
  /// /// than we need on some architectures.
  ///
  void increase_strong(size_type amount) noexcept {
    increase_common(amount);
    _total.fetch_add(amount, acq_rel);
  }

  void wait(size_type old,
      std::memory_order order = seq_cst) const noexcept {
    _total.wait(old, order);
  }
  void notify_one() noexcept { _total.notify_one(); }
  void notify_all() noexcept { _total.notify_all(); }

private:

  ///
  /// Common implementation of increase
  ///
  /// @param amount Amount to increase the accumulator
  ///
  /// These operations must be ordered before the increase in the accumulator,
  /// to ensure they are seen when another thread observes the accumulator.
  ///
  /// Note that these operations imply a read-for-ownership the accumulator's
  /// cacheline, so the weak version will not waste a snoop by obtaining the
  /// cacheline in shared state.
  ///
  void increase_common(size_type amount) noexcept {
    _recent += amount;
  }

              size_type  _recent; ///< Recent number
  std::atomic<size_type> _total;  ///< Total number
};

}

#endif
