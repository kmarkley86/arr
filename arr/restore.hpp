#ifndef ARR_RESTORE_HPP
#define ARR_RESTORE_HPP
//
// Copyright (c) 2012, 2015, 2021
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

#include "arr/special_member.hpp"
#include <type_traits>
#include <utility>

namespace arr {

/// \addtogroup scope_utils
/// @{

///
/// Restore the value of a variable at destruction
///
template <typename T> struct restore : noncopyable {

  ///
  /// Construct a restorer for a variable
  ///
  /// Copy the original value of the variable.
  ///
  restore(T& variable)
    noexcept(std::is_nothrow_copy_constructible<T>::value)
    : reference(variable), value(variable)
  {
  }

  ///
  /// Construct a restorer for a variable
  ///
  /// Provide a new value for the variable immediately, so the old value
  /// can be preserved with a move instead of a copy.
  ///
  restore(T& variable, T new_value)
    noexcept(std::is_nothrow_move_constructible<T>::value
        and  std::is_nothrow_move_assignable<T>::value)
    : reference(variable), value(std::move(variable))
  {
    variable = std::move(new_value);
  }

  ///
  /// Restore the variable from the saved value
  ///
  ~restore()
    noexcept(std::is_nothrow_move_assignable<T>::value)
  {
    reference = std::move(value);
  }

  restore(restore&&) = default;
  restore& operator=(restore&&) = default;

private:
  T& reference;
  T  value;
};

///
/// Automatically restore the value of a variable
///
template <typename T>
inline restore<T> make_restore(T& variable) {
  return restore<T>(variable);
}

///
/// Automatically restore the value of a variable, setting a new value now
///
template <typename T>
inline restore<T> make_restore(T& variable, T new_value) {
  return restore<T>(variable, std::move(new_value));
}

/// @}

}

#endif
