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

#include "arrtest/arrtest.hpp"
#include "arr/algorithm.hpp"
#include <functional>

UNIT_TEST_MAIN

SUITE(min) {

  TEST(min_rvalue) {
    CHECK_EQUAL(1, arr::min(1, 2));
    CHECK_EQUAL(1, arr::min(2, 1));
    CHECK_EQUAL(1, arr::min(1, 1));
    CHECK_EQUAL(1, arr::min(2, 1, 2));
    CHECK_EQUAL(1, arr::min(2, 1, 2, 2));
    CHECK_EQUAL(0, arr::min(2, 1, 2, 2, 0));
  }

  TEST(min_const_lvalue) {
    const int i = 1;
    const int j = 2;
    const int k = 1;
    CHECK_EQUAL(&i, &arr::min(i, j));
    CHECK_EQUAL(&i, &arr::min(j, i));
    CHECK_EQUAL(&i, &arr::min(i, k));
    CHECK_EQUAL(&k, &arr::min(k, i));
  }

  TEST(min_mutable_lvalue) {
    int i = 1;
    int j = 2;
    int k = 1;
    CHECK_EQUAL(&i, &arr::min(i, j));
    CHECK_EQUAL(&i, &arr::min(j, i));
    CHECK_EQUAL(&i, &arr::min(i, k));
    CHECK_EQUAL(&k, &arr::min(k, i));
    arr::min(i, k) = 3;
    CHECK_EQUAL(3, i);
    CHECK_EQUAL(1, k);
    k = 3;
    arr::min(k, i) = 4;
    CHECK_EQUAL(4, k);
    CHECK_EQUAL(3, i);
  }

  TEST(min_const_variadic) {
    const int i = 1;
    const int j = 2;
    const int k = 1;
    const int l = 2;
    CHECK_EQUAL(&i, &arr::min(j, i, k, l));
  }

  TEST(min_mutable_variadic) {
    int i = 1;
    int j = 2;
    int k = 1;
    int l = 2;
    CHECK_EQUAL(&i, &arr::min(j, i, k, l));
  }

}

SUITE(max) {

  TEST(max_rvalue) {
    CHECK_EQUAL(2, arr::max(1, 2));
    CHECK_EQUAL(2, arr::max(2, 1));
    CHECK_EQUAL(1, arr::max(1, 1));
    CHECK_EQUAL(2, arr::max(1, 2, 1));
    CHECK_EQUAL(2, arr::max(1, 2, 1, 1));
    CHECK_EQUAL(3, arr::max(2, 1, 2, 2, 3));
  }

  TEST(max_const_lvalue) {
    const int i = 1;
    const int j = 2;
    const int k = 1;
    CHECK_EQUAL(&j, &arr::max(i, j));
    CHECK_EQUAL(&j, &arr::max(j, i));
    CHECK_EQUAL(&k, &arr::max(i, k));
    CHECK_EQUAL(&i, &arr::max(k, i));
  }

  TEST(max_mutable_lvalue) {
    int i = 1;
    int j = 2;
    int k = 1;
    CHECK_EQUAL(&j, &arr::max(i, j));
    CHECK_EQUAL(&j, &arr::max(j, i));
    CHECK_EQUAL(&k, &arr::max(i, k));
    CHECK_EQUAL(&i, &arr::max(k, i));
    arr::max(i, k) = 3;
    CHECK_EQUAL(1, i);
    CHECK_EQUAL(3, k);
    i = 3;
    arr::max(k, i) = 4;
    CHECK_EQUAL(3, k);
    CHECK_EQUAL(4, i);
  }

  TEST(max_const_variadic) {
    const int i = 1;
    const int j = 2;
    const int k = 1;
    const int l = 2;
    CHECK_EQUAL(&l, &arr::max(i, j, l, k));
  }

  TEST(max_mutable_variadic) {
    int i = 1;
    int j = 2;
    int k = 1;
    int l = 2;
    CHECK_EQUAL(&l, &arr::max(i, j, l, k));
  }

}
