//
// Copyright (c) 2012, 2013, 2021
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
#include "arrtest/arrtest.hpp"
#include <iostream>
using namespace std;

UNIT_TEST_MAIN

inline void trace(const char * msg) {
  cout << msg << " ";
}

struct sentinel {
  ~sentinel() { cout << endl; }
};

struct foo {
  int value;
  ~foo() { trace("d "); }
  foo() : value(0) { trace("dc"); }
  foo(int x) : value(x) { trace("ic"); }
  foo(const foo&  peer) : value(peer.value) { trace("cc"); }
  foo(      foo&& peer) : value(peer.value) { trace("mc"); peer.value = 0; }
#if 1
  // FIXME make 2 classes so both can have tests...
  DEFINE_SWAP_COPY_ASSIGN(foo)
  DEFINE_SWAP_MOVE_ASSIGN(foo)
#else
  DEFINE_SWAP_ASSIGN(foo)
#endif
  DEFINE_RVALUE_SWAP(foo)
  DEFINE_FRIEND_SWAP(foo)
  void swap(foo& peer) noexcept {
    trace("ms");
    using std::swap;
    foo& self = *this;
    swap(self.value, peer.value);
  }
};

SUITE(constructors) {

  TEST(default) {
    sentinel s;
    foo x;
    CHECK_EQUAL(0, x.value);
  }

  TEST(initialize) {
    sentinel s;
    foo x(1);
    CHECK_EQUAL(1, x.value);
  }

  TEST(copy) {
    sentinel s;
    foo x(1);
    foo y(x);
    CHECK_EQUAL(1, x.value);
    CHECK_EQUAL(1, y.value);
  }

  TEST(move) {
    sentinel s;
    foo x(1);
    foo y(std::move(x));
    CHECK_EQUAL(0, x.value);
    CHECK_EQUAL(1, y.value);
  }

}

SUITE(try_swap) {

  TEST(member) {
    sentinel s;
    foo x(1);
    foo y(2);
    x.swap(y);
    CHECK_EQUAL(2, x.value);
    CHECK_EQUAL(1, y.value);
    y.swap(x);
    CHECK_EQUAL(1, x.value);
    CHECK_EQUAL(2, y.value);
  }

  TEST(rvalue) {
    sentinel s;
    foo x(1);
    x.swap(foo(2));
    CHECK_EQUAL(2, x.value);
    foo(3).swap(x);
    CHECK_EQUAL(3, x.value);
  }

  TEST(friend) {
    sentinel s;
    foo x(1);
    foo y(2);
    swap(foo(3), foo(4)); // Cannot check result, but should compile
    swap(     x, foo(5));
    CHECK_EQUAL(5, x.value);
    swap(foo(6),      y);
    CHECK_EQUAL(6, y.value);
    swap(     x,      y);
    CHECK_EQUAL(6, x.value);
    CHECK_EQUAL(5, y.value);
  }

  TEST(std) {
    sentinel s;
    foo x(1);
    foo y(2);
    std::swap(x, y);
    CHECK_EQUAL(2, x.value);
    CHECK_EQUAL(1, y.value);
  }

}

SUITE(assignment) {

  TEST(copy) {
    sentinel s;
    foo x(1);
    foo y(2);
    y = x;
    CHECK_EQUAL(1, x.value);
    CHECK_EQUAL(1, y.value);
  }

  TEST(self_assign_copy) {
    sentinel s;
    foo x(1);
    x = x;
    CHECK_EQUAL(1, x.value);
  }

  TEST(move1) {
    sentinel s;
    foo x(1);
    foo y(2);
    y = std::move(x);
    CHECK_EQUAL(2, x.value);
    CHECK_EQUAL(1, y.value);
  }

  TEST(move2) {
    sentinel s;
    foo x(1);
    x = foo(2);
    CHECK_EQUAL(2, x.value);
  }

  TEST(self_assign_move) {
    sentinel s;
    foo x(1);
    x = std::move(x);
    CHECK_EQUAL(1, x.value);
  }

}

SUITE(try_noexcept) {
  foo x(1);
  foo y(2);

  TEST(deduction) {
    cout << "Deduction of noexcept" << endl;
    cout << "dtor " << noexcept(x.~foo()) << endl;
    cout << "ctor() " << noexcept(foo()) << endl;
    cout << "ctor(x) " << noexcept(foo(1)) << endl;
    cout << "copy_ctor " << noexcept(foo(x)) << endl;
    cout << "move_ctor " << noexcept(foo(std::move(x))) << endl;
    cout << "copy_assign " << noexcept(x = y) << endl;
    cout << "move_assign " << noexcept(x = std::move(y)) << endl;
    cout << "member_swap " << noexcept(x.swap(y)) << endl;
    cout << "rvalue_swap " << noexcept(x.swap(std::move(y))) << endl;
    cout << "friend_swap " << noexcept(swap(x, y)) << endl;
    cout << "std_swap " << noexcept(std::swap(x, y)) << endl;
    cout << endl;
  }

}
