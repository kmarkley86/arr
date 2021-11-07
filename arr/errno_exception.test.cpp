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

#include "arr/errno_exception.hpp"
#include <utility>
#include <sstream>
#include <iostream>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;
using namespace arr;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: errno_exception" << endl;
  report_operations<errno_exception>(cout);
}

inline void thrower() {
  throw errno_exception{SOURCE_CONTEXT};
}

SUITE(ctor) {
  TEST(null) {
    errno = 1;
    errno_exception e;
    CHECK_EQUAL(1, e.value());
  }
  TEST(given) {
    errno_exception e(2);
    CHECK_EQUAL(2, e.value());
  }
  TEST(context) {
    errno_exception e(SOURCE_CONTEXT);
    CHECK_EQUAL(1, e.value());
  }
}

SUITE(ops) {
  const errno_exception x(3);
  TEST(mc) {
    errno_exception i(x);
    errno_exception e(std::move(i));
    CHECK_EQUAL(x.code(), e.code());
  }
  TEST(ma) {
    errno_exception i(x);
    errno_exception a;
    a = std::move(i);
    CHECK_EQUAL(x.code(), a.code());
  }
  TEST(sma) {
    errno_exception a(x);
    a = std::move(a);
    CHECK_EQUAL(x.code(), a.code());
  }
}

SUITE(usage) {
  TEST(basic) {
    try {
      thrower();
      CHECK_CATCH(errno_exception, e);
      static_cast<void>(e);
    }
  }
  TEST(context) {
    source_context context = SOURCE_CONTEXT;
    errno_exception e(context);
    CHECK_EQUAL(context.context(), e.context());
  }
}

SUITE(strings) {
  TEST(without_context) {
    errno_exception e(1);
    std::ostringstream s;
    s << "error #1: " << e.message();
    CHECK_EQUAL(s.str(), e.what());
  }
  TEST(with_context) {
    errno_exception e({"hello()", "world.cpp", 50}, 1);
    std::ostringstream s;
    s << "error #1: " << e.message();
    s << " [context: " << e.context() << "]";
    CHECK_EQUAL(s.str(), e.what());
  }
  TEST(show) {
    errno_exception e(SOURCE_CONTEXT, 1);
    cout << "Example: " << e.what() << endl;
  }
}
