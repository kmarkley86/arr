#ifndef ARR_SWAP_MACROS_HPP
#define ARR_SWAP_MACROS_HPP
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

#include <utility>

/// \addtogroup miscellaneous
/// @{

///
/// Copy assignment
///
/// Implementation uses copy construction and swap.
/// This is correct, but a custom implementation could be more efficient by
/// not storing to the temporary that will be immediately deleted.
///
#define DEFINE_SWAP_COPY_ASSIGN(T) \
T& operator=(const T& peer) { T(peer).swap(*this); return *this; }

///
/// Move assignment
///
/// Implementation uses swap.
/// This is correct, but a custom implementation could be more efficient by
/// not writing to the peer data members that don't hold resources.
///
#define DEFINE_SWAP_MOVE_ASSIGN(T) \
T& operator=(T&& peer) noexcept { \
  static_assert(noexcept(swap(peer)), "swap must be noexcept"); \
  swap(peer); \
  return *this; \
}

///
/// Unified assignment
///
/// Implementation uses swap.
/// This is correct, but a custom implementation could be more efficient by
/// not storing to the temporary that will be immediately deleted.
///
/// Separate definitions for copy and move assignment can be more efficient
/// by eliminating the move constructor call when the argument is an rvalue.
///
#define DEFINE_SWAP_ASSIGN(T) \
T& operator=(T peer) noexcept { \
  static_assert(noexcept(swap(peer)), "swap must be noexcept"); \
  swap(peer); \
  return *this; \
}

///
/// Rvalue swap
///
#define DEFINE_RVALUE_SWAP(T) \
void swap(T&& peer) noexcept { \
  static_assert(noexcept(swap(peer)), "swap must be noexcept"); \
  swap(peer); \
}

///
/// Friend swap
///
/// Define binary swap in terms of member swap.
///
#define DEFINE_FRIEND_SWAP(T) \
friend void swap(T&  x, T&  y) { x.swap(y); } \
friend void swap(T&& x, T&  y) { x.swap(y); } \
friend void swap(T&  x, T&& y) { x.swap(y); } \
friend void swap(T&& x, T&& y) { x.swap(y); }

/// @}

#endif
