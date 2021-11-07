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

#include "arr/context_exception.hpp"
#include <utility>
#include <iostream>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;
using namespace arr;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: context_exception" << endl;
  report_operations<context_exception>(cout);
}

inline void thrower() {
  throw context_exception{SOURCE_CONTEXT};
}

inline const void * voidify(const char * ptr) {
  return static_cast<const void *>(ptr);
}

SUITE(ctor) {
  TEST(null) {
    context_exception c;
    CHECK_EQUAL(voidify(nullptr), voidify(c.func));
    CHECK_EQUAL(voidify(nullptr), voidify(c.file));
    CHECK_EQUAL(0u, c.line);
  }
  TEST(given) {
    static const char func_name[] = "hello";
    static const char file_name[] = "world";
    context_exception c{source_context{func_name, file_name, 1}};
    CHECK_EQUAL(func_name, c.func);
    CHECK_EQUAL(file_name, c.file);
    CHECK_EQUAL(1u, c.line);
  }
}

SUITE(ops) {
  static const char func_name[] = "hello";
  static const char file_name[] = "world";
  const context_exception e{source_context{func_name, file_name, 1}};
  TEST(cc) {
    context_exception a{e};
    CHECK_EQUAL(voidify(e.func), voidify(a.func));
    CHECK_EQUAL(voidify(e.file), voidify(a.file));
    CHECK_EQUAL(e.line, a.line);
  }
  TEST(mc) {
    context_exception i{e};
    std::string before(i.format());
    context_exception a{std::move(i)};
    CHECK_EQUAL(voidify(e.func), voidify(a.func));
    CHECK_EQUAL(voidify(e.file), voidify(a.file));
    CHECK_EQUAL(e.line, a.line);
    CHECK_EQUAL(before, a.format());
  }
  TEST(ca) {
    context_exception a;
    a = e;
    CHECK_EQUAL(voidify(e.func), voidify(a.func));
    CHECK_EQUAL(voidify(e.file), voidify(a.file));
    CHECK_EQUAL(e.line, a.line);
  }
  TEST(ma) {
    context_exception i{e};
    context_exception a;
    std::string before(i.format());
    a = std::move(i);
    CHECK_EQUAL(voidify(e.func), voidify(a.func));
    CHECK_EQUAL(voidify(e.file), voidify(a.file));
    CHECK_EQUAL(e.line, a.line);
    CHECK_EQUAL(before, a.format());
  }
  TEST(sca) {
    context_exception a{e};
    std::string before(a.format());
    a = a;
    CHECK_EQUAL(voidify(e.func), voidify(a.func));
    CHECK_EQUAL(voidify(e.file), voidify(a.file));
    CHECK_EQUAL(e.line, a.line);
    CHECK_EQUAL(before, a.format());
  }
  TEST(sma) {
    context_exception a{e};
    std::string before(a.format());
    a = std::move(a);
    CHECK_EQUAL(voidify(e.func), voidify(a.func));
    CHECK_EQUAL(voidify(e.file), voidify(a.file));
    CHECK_EQUAL(e.line, a.line);
    CHECK_EQUAL(before, a.format());
  }
}

SUITE(usage) {
  TEST(basic) {
    try {
      thrower();
      CHECK_CATCH(context_exception, e);
      static_cast<void>(e);
    }
  }
  TEST(context) {
    source_context context = SOURCE_CONTEXT;
    context_exception e(context);
    CHECK_EQUAL(context.context(), e.context());
  }
  TEST(change_context) {
    source_context context1 = SOURCE_CONTEXT;
    source_context context2 = SOURCE_CONTEXT;
    context_exception e(context1);
    e = context2;
    CHECK_EQUAL(context2.context(), e.context());
  }
}

SUITE(strings) {
  TEST(format) {
    const context_exception e(SOURCE_CONTEXT);
    CHECK_EQUAL(e.context(), e.format());
  }
  TEST(what) {
    const context_exception e(SOURCE_CONTEXT);
    CHECK_EQUAL(e.context(), string(e.what()));
  }
  TEST(show) {
    const context_exception e(SOURCE_CONTEXT);
    cout << "Example: " << e.what() << endl;
  }
}
