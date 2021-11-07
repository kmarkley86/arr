#ifndef ARR_ALGORITHM
#define ARR_ALGORITHM
//
// Copyright (c) 2015, 2021
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

///
/// \file
/// \ingroup algorithms
///

namespace arr {

/// @name min
/// @{
///
/// minimum
///
/// Reference: C++11 standard §25.4.7
///
/// Requires: Type T is LessThanComparable
///
/// Returns: The smaller value
///
/// Remarks: When values are equivalent, returns the earliest
///

template <typename T>
constexpr const T& min(const T& a, const T& b)
noexcept(noexcept(std::declval<const T&>() < std::declval<const T&>())) {
  return (b < a) ? b : a;
}

template <typename T>
constexpr       T& min(      T& a,       T& b)
noexcept(noexcept(std::declval<      T&>() < std::declval<      T&>())) {
  return (b < a) ? b : a;
}

template <typename T, typename... Args>
constexpr const T& min(const T& a, const T& b, const Args&... args)
noexcept(noexcept(std::declval<const T&>() < std::declval<const T&>())) {
  return min((b < a) ? b : a, args...);
}

template <typename T, typename... Args>
constexpr       T& min(      T& a,       T& b,       Args&... args)
noexcept(noexcept(std::declval<      T&>() < std::declval<      T&>())) {
  return min((b < a) ? b : a, args...);
}

/// @}

/// @name max
/// @{
///
/// maximum
///
/// Reference: C++11 standard §25.4.7
///
/// Requires: Type T is LessThanComparable
///
/// Returns: The larger value
///
/// Remarks: When values are equivalent, returns the latest
///

template <typename T>
constexpr const T& max(const T& a, const T& b)
noexcept(noexcept(std::declval<const T&>() < std::declval<const T&>())) {
  return (b < a) ? a : b;
}

template <typename T>
constexpr       T& max(      T& a,       T& b)
noexcept(noexcept(std::declval<      T&>() < std::declval<      T&>())) {
  return (b < a) ? a : b;
}

template <typename T, typename... Args>
constexpr const T& max(const T& a, const T& b, const Args&... args)
noexcept(noexcept(std::declval<const T&>() < std::declval<const T&>())) {
  return max((b < a) ? a : b, args...);
}

template <typename T, typename... Args>
constexpr       T& max(      T& a,       T& b,       Args&... args)
noexcept(noexcept(std::declval<      T&>() < std::declval<      T&>())) {
  return max((b < a) ? a : b, args...);
}

/// @}

}

#endif
