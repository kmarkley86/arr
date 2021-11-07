#ifndef ARR_COPY_PTR_HPP
#define ARR_COPY_PTR_HPP
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

#include "arr/swap_macros.hpp"
#include "arr/clone_macros.hpp"
#include <utility>

namespace arr {

///
/// \ingroup miscellaneous
/// Copying smart pointer
///
/// This is a smart pointer that, when copied, uses the pointed-to object's
/// clone method or copy constructor to copy the object.
///
/// The clone method is preferred, if it exists, to avoid creating a sliced
/// object.
///
template <typename T> struct copy_ptr {
  typedef T* pointer;
  typedef T  element_type;
  ~copy_ptr() noexcept { delete ptr; }
  explicit copy_ptr(pointer p = nullptr) noexcept : ptr(p) { }
  copy_ptr(const copy_ptr&  peer)          : ptr(smart_copy(peer)) { }
  copy_ptr(      copy_ptr&& peer) noexcept : ptr(peer.release()) { }
  DEFINE_SWAP_COPY_ASSIGN(copy_ptr)
  DEFINE_SWAP_MOVE_ASSIGN(copy_ptr)
  DEFINE_RVALUE_SWAP(copy_ptr)
  DEFINE_FRIEND_SWAP(copy_ptr)
  element_type& operator* () const noexcept { return *ptr; }
  pointer       operator->() const noexcept { return  ptr; }
             operator bool() const noexcept { return nullptr != ptr; }
  void swap(copy_ptr& peer) noexcept { using std::swap; swap(ptr, peer.ptr); }
  pointer     get() const noexcept { return ptr; }
  pointer release()       noexcept { auto r = ptr; ptr = nullptr; return r; }
  void reset(pointer p)   noexcept { auto r = ptr; ptr = p;       delete r; }
private:
  pointer ptr;

  template <typename U, typename std::enable_if<
    not trait::is_cloneable<typename U::element_type>::value, int>::type = 0>
  pointer smart_copy(const U& peer) const { return new element_type(*peer); }

  template <typename U, typename std::enable_if<
        trait::is_cloneable<typename U::element_type>::value, int>::type = 0>
  pointer smart_copy(const U& peer) const { return peer->clone(); }

};

}

namespace std {
  template <typename T>
  inline void swap(arr::copy_ptr<T>& x, arr::copy_ptr<T>& y) { swap(x, y); }
}

#endif
