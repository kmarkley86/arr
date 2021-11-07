#ifndef ARR_BASIC_PTR_HPP
#define ARR_BASIC_PTR_HPP
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

#include <cstddef>
#include <utility>

namespace arr {

///
/// \ingroup miscellaneous
/// Basic pointer
///
/// This is a smart pointer that explicitly does not own the pointed-to memory.
/// It is provided for the purpose of replacing ambiguous-looking raw pointers
/// with a type that has clear ownership semantics.
///
template <typename T> struct basic_ptr {
  using pointer = T*;
  using element_type = T;

  constexpr basic_ptr()               noexcept : ptr(nullptr) { }
  constexpr basic_ptr(std::nullptr_t) noexcept : ptr(nullptr) { }
  constexpr basic_ptr(pointer p)      noexcept : ptr(p) { }

  pointer release() noexcept { auto r = ptr; ptr = nullptr; return r; }
  void reset(pointer p = pointer()) noexcept { ptr = p; }
  void swap(basic_ptr& peer) noexcept { using std::swap; swap(ptr, peer.ptr); }
  friend void swap(basic_ptr& a, basic_ptr& b) noexcept { a.swap(b); }

  pointer                 get() const noexcept { return  ptr; }
  element_type& operator*    () const noexcept { return *ptr; }
  pointer       operator->   () const noexcept { return  ptr; }
  explicit      operator bool() const noexcept { return  nullptr != ptr; }
  element_type& operator[](std::size_t i) const noexcept { return get()[i]; }

  friend bool operator==(const basic_ptr& p, std::nullptr_t) noexcept {
    return p.get() == nullptr;
  }
  friend bool operator==(std::nullptr_t, const basic_ptr& p) noexcept {
    return nullptr == p.get();
  }

  friend bool operator!=(const basic_ptr& p, std::nullptr_t) noexcept {
    return p.get() != nullptr;
  }
  friend bool operator!=(std::nullptr_t, const basic_ptr& p) noexcept {
    return nullptr != p.get();
  }

private:
  pointer ptr;
};

template <typename T1, typename T2> inline bool
operator==(const basic_ptr<T1>& a, const basic_ptr<T2>& b) noexcept {
  return a.get() == b.get();
}

template <typename T1, typename T2> inline bool
operator!=(const basic_ptr<T1>& a, const basic_ptr<T2>& b) noexcept {
  return a.get() != b.get();
}

}

#endif
