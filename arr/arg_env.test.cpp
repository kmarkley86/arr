//
// Copyright (c) 2012, 2013, 2021, 2022, 2025
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

#include "arr/arg_env.hpp"
#include "arrtest/arrtest.hpp"
#include <iostream>
using namespace std;
using namespace arr;

UNIT_TEST_MAIN

inline const void * voidify(const char * ptr) {
  return static_cast<const void *>(ptr);
}

SUITE(try_arguments) {

  TEST(void) {
    arguments a;
    CHECK_EQUAL(static_cast<decltype(a.size())>(0), a.size());
    auto temp = a.as_argv();
    const char * const * p = temp.get();
    CHECK_EQUAL(voidify(nullptr), voidify(p[0]));
  }

  TEST(zero) {
    const char * const argv[] = { nullptr };
    arguments a(argv);
    CHECK_EQUAL(static_cast<decltype(a.size())>(0), a.size());
    auto temp = a.as_argv();
    const char * const * p = temp.get();
    CHECK_EQUAL(voidify(nullptr), voidify(p[0]));
  }

  TEST(one) {
    const char * const argv[] = { "string", nullptr };
    arguments a(argv);
    CHECK_EQUAL(static_cast<decltype(a.size())>(1), a.size());
    auto temp = a.as_argv();
    const char * const * p = temp.get();
    CHECK_STRINGS("string", p[0]);
    CHECK_EQUAL(voidify(nullptr), voidify(p[1]));
  }

  TEST(two) {
    const char * const argv[] = { "string", "arg1", nullptr };
    arguments a(argv);
    CHECK_EQUAL(static_cast<decltype(a.size())>(2), a.size());
    auto temp = a.as_argv();
    const char * const * p = temp.get();
    CHECK_STRINGS("string", p[0]);
    CHECK_STRINGS("arg1"  , p[1]);
    CHECK_EQUAL(voidify(nullptr), voidify(p[2]));
  }

  TEST(add) {
    const char * const argv[] = { "arg1", "arg2", nullptr };
    arguments a(argv);
    a.emplace_front("process");
    CHECK_EQUAL(static_cast<decltype(a.size())>(3), a.size());
    auto temp = a.as_argv();
    const char * const * p = temp.get();
    CHECK_STRINGS("process", p[0]);
    CHECK_STRINGS("arg1"   , p[1]);
    CHECK_STRINGS("arg2"   , p[2]);
    CHECK_EQUAL(voidify(nullptr), voidify(p[3]));
  }

  TEST(swap) {
    const char * const X[] = { "a", "b", nullptr };
    const char * const Y[] = { "c", "d", nullptr };
    arguments x(X);
    arguments y(Y);
    swap(x, y);
    auto temp_x = x.as_argv();
    auto temp_y = y.as_argv();
    const char * const * from_x = temp_x.get();
    const char * const * from_y = temp_y.get();
    CHECK_STRINGS("c", from_x[0]);
    CHECK_STRINGS("a", from_y[0]);
  }

}

SUITE(try_environment) {

  TEST(zero) {
    const char * const envp[] = { nullptr };
    environment e(envp);
    CHECK_EQUAL(static_cast<decltype(e.size())>(0), e.size());
    const char * const * p = e;
    CHECK_EQUAL(voidify(nullptr), voidify(p[0]));
  }

  TEST(one) {
    const char * const envp[] = { "FOO", nullptr };
    environment e(envp);
    CHECK_EQUAL(static_cast<decltype(e.size())>(1), e.size());
    const char * const * p = e;
    CHECK_STRINGS("FOO=", p[0]);
    CHECK_EQUAL(voidify(nullptr), voidify(p[1]));
  }

  TEST(two) {
    const char * const envp[] = { "FOO", "BAR=baz", nullptr };
    environment e(envp);
    CHECK_EQUAL(static_cast<decltype(e.size())>(2), e.size());
    const char * const * p = e;
    CHECK_STRINGS("FOO="   , p[1]);
    CHECK_STRINGS("BAR=baz", p[0]);
    CHECK_EQUAL(voidify(nullptr), voidify(p[2]));
  }

  TEST(add) {
    const char * const envp[] = { "FOO", "BAR=baz", nullptr };
    environment e(envp);
    e["XYZZY"] = "yay";
    CHECK_EQUAL(static_cast<decltype(e.size())>(3), e.size());
    const char * const * p = e;
    CHECK_STRINGS("FOO="     , p[1]);
    CHECK_STRINGS("BAR=baz"  , p[0]);
    CHECK_STRINGS("XYZZY=yay", p[2]);
    CHECK_EQUAL(voidify(nullptr), voidify(p[3]));
  }

  TEST(void) {
    environment e;
    auto size = e.size();
    const char * const * p = e;
    CHECK_EQUAL(voidify(nullptr), voidify(p[size]));
  }
}
