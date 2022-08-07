#ifndef ARR_MASK_HPP
#define ARR_MASK_HPP
//
// Copyright (c) 2013, 2021, 2022
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

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace arr {

/// \addtogroup miscellaneous
/// @{

template <typename T>
constexpr auto full_width() {
  return
    std::numeric_limits<T>::digits +
    std::numeric_limits<T>::is_signed;
}

template <typename T>
constexpr bool less_than_full_width(unsigned num_bits) {
  return num_bits < full_width<T>();
}

template <typename T>
constexpr bool greater_than_full_width(unsigned num_bits) {
  return full_width<T>() < num_bits;
}

template <typename T>
constexpr bool equals_full_width(unsigned num_bits) {
  return full_width<T>() == num_bits;
}

///
/// All-zeros value
///
/// @return A value of type \c T with all bits set to \c '0'
///
template <typename T>
constexpr T all_zeros() {
  return T(0u);
}

///
/// All-ones value
///
/// @return A value of type \c T with all bits set to \c '1'
///
template <typename T>
constexpr T all_ones() {
  return static_cast<T>(~all_zeros<T>());
}

///
/// Bit mask of zeros in lowest position (partial values only)
///
/// @param num_zeros Number of \c '0' bits; must be fewer than the width of \c T
/// @return A \c T with the lowest \c num_zeros set to \c '0', rest \c '1'
///
template <typename T>
constexpr T partial_low_zeros(unsigned num_zeros) {
  return static_cast<T>(all_ones<T>() << num_zeros);
}

///
/// Bit mask of ones in lowest position (partial values only)
///
/// @param num_ones Number of \c '1' bits; must be fewer than the width of \c T
/// @return A \c T with the lowest \c num_ones set to \c '1', rest \c '0'
///
template <typename T>
constexpr T partial_low_ones(unsigned num_ones) {
  return static_cast<T>(~partial_low_zeros<T>(num_ones));
}

///
/// Bit mask of zeros in lowest position
///
/// @param num_zeros Number of \c '0' bits
/// @return A \c T with the lowest \c num_zeros bits set to \c '0', rest \c '1'
///
/// If \c num_zeros is greater than the number of bits in type \c T,
/// the return value will have all bits zero.
///
template <typename T>
constexpr T low_zeros(unsigned num_zeros) {
  if (less_than_full_width<T>(num_zeros)) {
    return partial_low_zeros<T>(num_zeros);
  } else {
    return all_zeros<T>();
  }
}

///
/// Bit mask of ones in lowest position
///
/// @param num_ones Number of \c '1' bits
/// @return A \c T with the lowest \c num_ones bits set to \c '1', rest \c '0'
///
/// If \c num_ones is greater than the number of bits in type \c T,
/// the return value will have all bits one.
///
template <typename T>
constexpr T low_ones(unsigned num_ones) {
  return static_cast<T>(~low_zeros<T>(num_ones));
}

///
/// Bit mask of zeros in lowest position (checks argument validity)
///
/// @param num_zeros Number of \c '0' bits
/// @return A \c T with the lowest \c num_zeros bits set to \c '0', rest \c '1'
///
/// If \c num_zeros is greater than the number of bits in type \c T,
/// throw std::invalid_argument.
///
template <typename T>
constexpr T checked_low_zeros(unsigned num_zeros) {
  if (greater_than_full_width<T>(num_zeros)) {
    throw std::invalid_argument("Exceeds data type width");
  } else {
    return low_zeros<T>(num_zeros);
  }
}

///
/// Bit mask of ones in lowest position (checks argument validity)
///
/// @param num_ones Number of \c '1' bits
/// @return A \c T with the lowest \c num_ones bits set to \c '1', rest \c '0'
///
/// If \c num_ones is greater than the number of bits in type \c T,
/// throw std::invalid_argument.
///
template <typename T>
constexpr T checked_low_ones(unsigned num_ones) {
  return static_cast<T>(~checked_low_zeros<T>(num_ones));
}

///
/// Bit mask of ones
///
/// @param width Number of \c '1' bits
/// @param position Bit position of lowest \c '1' bit
/// @return A \c T with \c width \c '1' bits starting at bit \c position
///
/// If the mask is out-of-range for type \c T, it will be truncated.
///
template <typename T>
constexpr T mask_width_position(unsigned width, unsigned position) {
  if (less_than_full_width<T>(position)) {
    return static_cast<T>(low_ones<T>(width) << position);
  } else {
    return all_zeros<T>();
  }
}

///
/// Bit mask of ones (checks argument validity)
///
/// @param width Number of \c '1' bits
/// @param position Bit position of lowest \c '1' bit
/// @return A \c T with \c width \c '1' bits starting at bit \c position
///
/// If the mask is out-of-range for type \c T, throw std::invalid_argument.
///
template <typename T>
constexpr T checked_mask_width_position(unsigned width, unsigned position) {
  if (greater_than_full_width<T>(width+position)) {
    throw std::invalid_argument("Exceeds data type width");
  } else {
    return mask_width_position<T>(width, position);
  }
}


///
/// Bit mask (ordered arguments)
///
/// @param high_bit Highest bit postion to make a \c '1'
/// @param  low_bit Lowest bit postion to make a \c '1'
/// @return A value of type \c T with bits [high_bit:low_bit] set to \c '1'
///
/// No argument checks are performed.  The result is undefined if high_bit
/// is less than low_bit.  If high_bit does not exist in type \c T, the mask
/// will be truncated.
///
template <typename T>
constexpr T mask_ordered_pair(unsigned high_bit, unsigned low_bit) {
  return mask_width_position<T>(high_bit-low_bit+1u, low_bit);
}

///
/// Bit mask (ordered arguments, checks argument validity)
///
/// @param high_bit Highest bit postion to make a \c '1'
/// @param  low_bit Lowest bit postion to make a \c '1'
/// @return A value of type \c T with bits [high_bit:low_bit] set to \c '1'
///
/// If the mask is out-of-range for type \c T, throw std::invalid_argument.
///
template <typename T>
constexpr T checked_mask_ordered_pair(unsigned high_bit, unsigned low_bit) {
  return checked_mask_width_position<T>(high_bit-low_bit+1u, low_bit);
}

///
/// Bit mask (unordered arguments)
///
/// @param x High or low bit position to make a \c '1'
/// @param y High or low bit position to make a \c '1'
/// @return A value of type \c T with bits [x:y] set to \c '1'
///
/// No argument checks are performed.  The result is undefined if high_bit
/// is less than low_bit.  If high_bit does not exist in type \c T, the mask
/// will be truncated.
///
template <typename T>
constexpr T mask_unordered_pair(unsigned x, unsigned y) {
  auto [lo, hi] = std::minmax(x, y);
  return mask_ordered_pair<T>(hi, lo);
}

///
/// Bit mask (unordered arguments, checks argument validity)
///
/// @param x High or low bit position to make a \c '1'
/// @param y High or low bit position to make a \c '1'
/// @return A value of type \c T with bits [x:y] set to \c '1'
///
/// If the mask is out-of-range for type \c T, throw std::invalid_argument.
///
template <typename T>
constexpr T checked_mask_unordered_pair(unsigned x, unsigned y) {
  auto [lo, hi] = std::minmax(x, y);
  return checked_mask_ordered_pair<T>(hi, lo);
}

/// @}

}

#endif
