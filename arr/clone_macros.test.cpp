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

#include "arr/clone_macros.hpp"
#include "arrtest/arrtest.hpp"
#include <typeinfo>
using namespace std;

UNIT_TEST_MAIN

struct foo {
  int value;
  virtual ~foo() { }
  foo() : value(0) { }
  foo(int x) : value(x) { }
  foo(const foo&  peer) : value(peer.value) { }
  foo(      foo&& peer) : value(peer.value) { peer.value = 0; }
  DEFINE_CLONE
};

struct bar : foo {
  bar(int x) : foo(x) { }
  DEFINE_CLONE
};

struct no_clone {
  int value;
};

SUITE(try_clone) {

  TEST(normal) {
    foo x(3);
    foo * y = x.clone();
    CHECK_EQUAL(x.value, y->value);
    delete y;
  }

  TEST(retain_derived_type) {
    bar x(3);
    // Must be able to clone and get a derived pointer (not a base pointer)
    bar * y = x.clone();
    CHECK_EQUAL(x.value, y->value);
    delete y;
  }

  TEST(from_base_pointer) {
    bar x(3);
    // Must get a derived pointer when cloning from base pointer
    foo * base_ptr = &x;
    foo * y = base_ptr->clone();
    CHECK_EQUAL(x.value, y->value);
    CHECK(typeid( x) == typeid(*y));
    delete y;
  }

}

SUITE(trait) {
  TEST(all) {
    CHECK_EQUAL(false, arr::trait::is_cloneable<int>::value);
    CHECK_EQUAL(false, arr::trait::is_cloneable<no_clone>::value);
    CHECK_EQUAL(true , arr::trait::is_cloneable<foo>::value);
    CHECK_EQUAL(true , arr::trait::is_cloneable<bar>::value);
  }
}
