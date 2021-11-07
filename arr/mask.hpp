#ifndef ARR_MASK_HPP
#define ARR_MASK_HPP
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

#include <limits>
#include <stdexcept>

namespace arr {

/// \addtogroup miscellaneous
/// @{

///
/// All-ones value
///
/// @return A value of type \c T with all bits set to \c '1'
///
template <typename T> inline constexpr
T all_ones() {
  return static_cast<T>(~T(0));
}

///
/// Bit mask in lowest position (partial values only)
///
/// @param num_bits Number of \c '1' bits; must be fewer than the width of \c T
/// @return A value of type \c T with the lowest \c num_bits bits set to \c '1'
///
/// §iso.5.8/1 "The behavior is undefined if the right operand [of a shift
/// operator] is negative, or greater than or equal to the length in bits of
/// the promoted left operand."
///
/// The behavior is undefined for signed types (§iso.5.8/2), although an
/// implementation is likely to do what we want.
///
template <typename T> inline constexpr
T partial_low_mask(unsigned num_bits) {
  return static_cast<T>(~(all_ones<T>() << num_bits));
}

///
/// Bit mask in lowest position
///
/// @param num_bits Number of \c '1' bits
/// @return A value of type \c T with the lowest \c num_bits bits set to \c '1'
///
/// If \c num_bits is greater than the number of bits in type \c T,
/// the return value will have all bits set.
///
template <typename T> inline constexpr
T low_mask(unsigned num_bits) {
  return num_bits >=
      std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed
    ? all_ones<T>()
    : partial_low_mask<T>(num_bits);
}

///
/// Bit mask in lowest position (checks argument validity)
///
/// @param num_bits Number of \c '1' bits
/// @return A value of type \c T with the lowest \c num_bits bits set to \c '1'
///
/// If \c num_bits is greater than the number of bits in type \c T,
/// an std::invalid_argument exception will be thrown.
///
template <typename T> inline constexpr
T low_mask_checked(unsigned num_bits) {
  return
      num_bits >
      std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed
    ? throw std::invalid_argument("Data type is not wide enough")
    : low_mask<T>(num_bits);
}

///
/// Bit mask (ordered arguments)
///
/// @param high_bit Highest bit postion to make a \c '1'
/// @param  low_bit Lowest bit postion to make a \c '1'
/// @return A value of type \c T with bits [high_bit:low_bit] set to \c '1'
///
/// No argument checks are performed.  The result is undefined if high_bit
/// is less than low_bit or if neither exists in type \c T.  If high_bit
/// does not exist in type \c T, it will be treated as the uppermost bit.
///
template <typename T> inline constexpr
T mask(unsigned high_bit, unsigned low_bit) {
  return static_cast<T>(
      low_mask<T>(high_bit+1) ^ partial_low_mask<T>(low_bit));
}

///
/// Bit mask (ordered arguments, checked)
///
/// @param high_bit Highest bit postion to make a \c '1'
/// @param  low_bit Lowest bit postion to make a \c '1'
/// @return A value of type \c T with bits [high_bit:low_bit] set to \c '1'
///
/// The result is undefined if high_bit is less than low_bit.
///
/// An std::invalid_argument exception will be thrown if high_bit
/// does not exist in type \c T.
///
template <typename T> inline constexpr
T mask_checked(unsigned high_bit, unsigned low_bit) {
  return static_cast<T>(
      low_mask_checked<T>(high_bit+1) ^ partial_low_mask<T>(low_bit));
}

///
/// Bit mask (unordered arguments)
///
/// @param x High or low bit position to make a \c '1'
/// @param y High or low bit position to make a \c '1'
/// @return A value of type \c T with bits [x:y] set to \c '1'
///
/// No argument checks are performed.  The result is undefined if neither
/// argument bit exists in type \c T.  If the larger argument bit does not
/// exist in type \c T, it will be treated as the uppermost bit.
///
template <typename T> inline constexpr
T mask_unordered(unsigned x, unsigned y) {
  return x < y ? mask<T>(y, x) : mask<T>(x, y);
}

///
/// Bit mask (unordered arguments, checked)
///
/// @param x High or low bit position to make a \c '1'
/// @param y High or low bit position to make a \c '1'
/// @return A value of type \c T with bits [x:y] set to \c '1'
///
/// An std::invalid_argument exception will be thrown if either bit position
/// does not exist in type \c T.  The arguments may be given in any order.
///
template <typename T> inline constexpr
T mask_unordered_checked(unsigned x, unsigned y) {
  return x < y ? mask_checked<T>(y, x) : mask_checked<T>(x, y);
}

/// @}

}

#endif
