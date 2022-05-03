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

#include "arr/copy_ptr.hpp"
#include "arrtest/arrtest.hpp"
#include <typeinfo>
using namespace std;

UNIT_TEST_MAIN

struct uncloneable {
  int value;
  ~uncloneable() { }
  uncloneable() : value(0) { }
  uncloneable(int x) : value(x) { }
  uncloneable(const uncloneable&  peer) : value(peer.value) { }
  uncloneable(      uncloneable&& peer) : value(peer.value) { peer.value = 0; }
  uncloneable& operator=(int x) { value = x; return *this; }
  operator int() { return value; }
};

struct cloneable {
  int value;
  virtual ~cloneable() { }
  cloneable() : value(0) { }
  cloneable(int x) : value(x) { }
  cloneable(const cloneable&  peer) : value(peer.value) { }
  cloneable(      cloneable&& peer) : value(peer.value) { peer.value = 0; }
  cloneable& operator=(int x) { value = x; return *this; }
  operator int() { return value; }
  DEFINE_CLONE
};

struct sub : cloneable {
  sub(int x = 0) : cloneable(x) { }
  DEFINE_CLONE
};

SUITE(pod) {
  TEST(all) {
    arr::copy_ptr<int> p(new int);
    CHECK_EQUAL(true, p);
    *p = 1;
    arr::copy_ptr<int> q(p);
    CHECK_EQUAL(false, q.get() == p.get());
    CHECK_EQUAL(1, *p);
    CHECK_EQUAL(1, *q);
    *p = 2;
    CHECK_EQUAL(2, *p);
    CHECK_EQUAL(1, *q);
  }
}

SUITE(not_uncloneable) {
  TEST(all) {
    CHECK_EQUAL(false, arr::trait::is_cloneable<uncloneable>::value);
    arr::copy_ptr<uncloneable> p(new uncloneable);
    CHECK_EQUAL(true, p);
    *p = 1;
    arr::copy_ptr<uncloneable> q(p);
    CHECK_EQUAL(false, q.get() == p.get());
    CHECK_EQUAL(1, static_cast<int>(*p));
    CHECK_EQUAL(1, static_cast<int>(*q));
    *p = 2;
    CHECK_EQUAL(2, static_cast<int>(*p));
    CHECK_EQUAL(1, static_cast<int>(*q));
  }
}

SUITE(is_cloneable) {
  TEST(regular) {
    CHECK_EQUAL(true, arr::trait::is_cloneable<cloneable>::value);
    arr::copy_ptr<cloneable> p(new cloneable);
    CHECK_EQUAL(true, p);
    *p = 1;
    arr::copy_ptr<cloneable> q(p);
    CHECK_EQUAL(false, q.get() == p.get());
    CHECK_EQUAL(1, static_cast<int>(*p));
    CHECK_EQUAL(1, static_cast<int>(*q));
    *p = 2;
    CHECK_EQUAL(2, static_cast<int>(*p));
    CHECK_EQUAL(1, static_cast<int>(*q));
  }
  TEST(sub) {
    CHECK_EQUAL(true, arr::trait::is_cloneable<sub>::value);
    arr::copy_ptr<cloneable> p(new sub);        // Different types
    CHECK_EQUAL(true, p);
    *p = 1;
    arr::copy_ptr<cloneable> q(p);
    CHECK_EQUAL(false, q.get() == p.get());
    CHECK_EQUAL(1, static_cast<int>(*p));
    CHECK_EQUAL(1, static_cast<int>(*q));
    *p = 2;
    CHECK_EQUAL(2, static_cast<int>(*p));
    CHECK_EQUAL(1, static_cast<int>(*q));
    sub temp;
    auto& P = *p;
    auto& Q = *q;
    CHECK_EQUAL(typeid(temp).name(), typeid(P).name());
    CHECK_EQUAL(typeid(temp).name(), typeid(Q).name());
  }
}
