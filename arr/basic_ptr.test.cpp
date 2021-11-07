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

#include "arr/basic_ptr.hpp"
#include "arrtest/arrtest.hpp"

UNIT_TEST_MAIN

SUITE(creation) {

  TEST(default) {
    auto p = arr::basic_ptr<int>();
    CHECK_EQUAL(static_cast<int*>(nullptr), p.get());
  }

  TEST(nullptr) {
    auto p = arr::basic_ptr<int>(nullptr);
    CHECK_EQUAL(static_cast<int*>(nullptr), p.get());
  }

  TEST(regular) {
    int i;
    auto p = arr::basic_ptr<int>(&i);
    CHECK_EQUAL(&i, p.get());
  }

}

SUITE(assignment) {

  TEST(nullptr) {
    int i;
    auto p = arr::basic_ptr<int>(&i);
    p = nullptr;
    CHECK_EQUAL(static_cast<int*>(nullptr), p.get());
  }

  TEST(raw) {
    int i;
    auto p = arr::basic_ptr<int>();
    p = &i;
    CHECK_EQUAL(&i, p.get());
  }

  TEST(regular) {
    int i;
    auto p = arr::basic_ptr<int>();
    auto q = arr::basic_ptr<int>(&i);
    p = q;
    CHECK_EQUAL(&i, p.get());
    CHECK_EQUAL(&i, q.get());
  }

}

SUITE(modifiers) {

  TEST(release) {
    int i;
    auto p = arr::basic_ptr<int>(&i);
    CHECK_EQUAL(&i, p.release());
    CHECK_EQUAL(static_cast<int*>(nullptr), p.get());
  }

  TEST(reset) {
    int i;
    auto p = arr::basic_ptr<int>();
    p.reset(&i);
    CHECK_EQUAL(&i, p.get());
    p.reset();
    CHECK_EQUAL(static_cast<int*>(nullptr), p.get());
  }

  TEST(member_swap) {
    int i, j;
    auto p = arr::basic_ptr<int>(&i);
    auto q = arr::basic_ptr<int>(&j);
    p.swap(q);
    CHECK_EQUAL(&j, p.get());
    CHECK_EQUAL(&i, q.get());
  }

  TEST(friend_swap) {
    int i, j;
    auto p = arr::basic_ptr<int>(&i);
    auto q = arr::basic_ptr<int>(&j);
    swap(p, q);
    CHECK_EQUAL(&j, p.get());
    CHECK_EQUAL(&i, q.get());
  }

}

SUITE(observers) {

  TEST(dereference) {
    int i;
    auto p = arr::basic_ptr<int>(&i);
    CHECK_EQUAL(&i, &p.operator*());
  }

  TEST(arrow) {
    int i;
    auto p = arr::basic_ptr<int>(&i);
    CHECK_EQUAL(&i, p.operator->());
  }

  TEST(bool) {
    int i;
    auto p = arr::basic_ptr<int>(&i);
    auto q = arr::basic_ptr<int>();
    CHECK_EQUAL(true , static_cast<bool>(p));
    CHECK_EQUAL(false, static_cast<bool>(q));
    if (p) {
      CHECK_EQUAL(true, true);
    } else {
      CHECK_EQUAL(true, false);
    }
    if (q) {
      CHECK_EQUAL(true, false);
    } else {
      CHECK_EQUAL(true, true);
    }
  }

  TEST(index) {
    int i[2];
    auto p = arr::basic_ptr<int>(i);
    CHECK_EQUAL(i+0, &p[0]);
    CHECK_EQUAL(i+1, &p[1]);
  }

}

SUITE(comparisons) {

  TEST(nullptr_equal) {
    int i;
    auto p = arr::basic_ptr<int>(&i);
    auto q = arr::basic_ptr<int>();
    CHECK_EQUAL(false, p == nullptr);
    CHECK_EQUAL(false, nullptr == p);
    CHECK_EQUAL(true , q == nullptr);
    CHECK_EQUAL(true , nullptr == q);
  }

  TEST(nullptr_unequal) {
    int i;
    auto p = arr::basic_ptr<int>(&i);
    auto q = arr::basic_ptr<int>();
    CHECK_EQUAL(true , p != nullptr);
    CHECK_EQUAL(true , nullptr != p);
    CHECK_EQUAL(false, q != nullptr);
    CHECK_EQUAL(false, nullptr != q);
  }

  TEST(regular_equal) {
    struct A { };
    struct B : A { };
    A a;
    B b;
    auto p = arr::basic_ptr<A>(&b);
    auto q = arr::basic_ptr<B>(&b);
    auto r = arr::basic_ptr<A>(&a);
    CHECK_EQUAL(true , p == q);
    CHECK_EQUAL(true , q == p);
    CHECK_EQUAL(false, p == r);
    CHECK_EQUAL(false, q == r);
  }

  TEST(regular_unequal) {
    struct A { };
    struct B : A { };
    A a;
    B b;
    auto p = arr::basic_ptr<A>(&b);
    auto q = arr::basic_ptr<B>(&b);
    auto r = arr::basic_ptr<A>(&a);
    CHECK_EQUAL(false, p != q);
    CHECK_EQUAL(false, q != p);
    CHECK_EQUAL(true , p != r);
    CHECK_EQUAL(true , q != r);
  }

}
