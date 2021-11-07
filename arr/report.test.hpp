#ifndef ARR_OPERATIONS_HPP
#define ARR_OPERATIONS_HPP
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

#include <type_traits>
#include <iostream>

namespace arr {

template <typename T> void report_operations(std::ostream& o) {
  o << std::is_default_constructible<T>::value << " is_default_constructible\n";
  o << std::is_destructible<T>::value          << " is_destructible\n";
  o << std::is_copy_constructible<T>::value    << " is_copy_constructible\n";
  o << std::is_move_constructible<T>::value    << " is_move_constructible\n";
  o << std::is_copy_assignable<T>::value       << " is_copy_assignable\n";
  o << std::is_move_assignable<T>::value       << " is_move_assignable\n";
  o << std::is_nothrow_default_constructible<T>::value << " is_nothrow_default_constructible\n";
//o << std::is_nothrow_destructible<T>::value          << " is_nothrow_destructible\n";
  o << std::is_nothrow_copy_constructible<T>::value    << " is_nothrow_copy_constructible\n";
  o << std::is_nothrow_move_constructible<T>::value    << " is_nothrow_move_constructible\n";
  o << std::is_nothrow_copy_assignable<T>::value       << " is_nothrow_copy_assignable\n";
  o << std::is_nothrow_move_assignable<T>::value       << " is_nothrow_move_assignable\n";
/*
  o << std::is_trivially_default_constructible<T>::value << " is_trivially_default_constructible\n";
  o << std::is_trivially_destructible<T>::value          << " is_trivially_destructible\n";
  o << std::is_trivially_copy_constructible<T>::value    << " is_trivially_copy_constructible\n";
  o << std::is_trivially_move_constructible<T>::value    << " is_trivially_move_constructible\n";
  o << std::is_trivially_copy_assignable<T>::value       << " is_trivially_copy_assignable\n";
  o << std::is_trivially_move_assignable<T>::value       << " is_trivially_move_assignable\n";
*/
}

}

#endif
