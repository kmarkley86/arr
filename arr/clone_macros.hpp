#ifndef ARR_CLONE_MACROS_HPP
#define ARR_CLONE_MACROS_HPP
//
// Copyright (c) 2012, 2013, 2015, 2021
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

#include <type_traits>

/// \addtogroup miscellaneous
/// @{

///
/// Clone method
///
/// There is no good way to implement clone() in a base class and have it
/// automatically work for all derived classes via the CRTP.  It can almost
/// be done, but the experts say it fails for multiple inheritance:
///
/// http://compgroups.net/comp.lang.c++.moderated/covariant-return-types-with-crtp/
///
/// So, have a macro-based implementation.  Unfortunately, all derived classes
/// need to use this macro, too.
///

#define DEFINE_CLONE \
virtual auto clone() const -> typename \
    std::remove_cv<typename std::decay<decltype(*this)>::type>::type * { \
  return new typename \
    std::remove_cv<typename std::decay<decltype(*this)>::type>::type(*this); \
}

/// @}

namespace arr {
namespace trait {

template <typename T>
struct class_has_clone_member_function {
  template <T* (T::*)() const> struct signature;
  template <typename U> static std::true_type  check(signature<&U::clone>*);
  template <typename U> static std::false_type check(...);
  static constexpr bool value = decltype(check<T>(nullptr))::value;
};

///
/// \ingroup miscellaneous
/// Type trait indicating whether a type has a clone member function
///
template <typename T, bool is_class_type = std::is_class<T>::value>
struct is_cloneable : std::false_type { };

template <typename T>
struct is_cloneable<T, true>
  : std::integral_constant<bool, class_has_clone_member_function<T>::value> { };

}
}

#endif
