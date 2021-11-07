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

#include "arr/source_context.hpp"
#include <utility>
#include <iostream>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;
using namespace arr;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: source_context" << endl;
  report_operations<source_context>(cout);
}

SUITE(ctor) {
  TEST(null) {
    source_context c{nullptr, nullptr, 0};
    CHECK_EQUAL(static_cast<void *>(nullptr), c.func);
    CHECK_EQUAL(static_cast<void *>(nullptr), c.file);
    CHECK_EQUAL(static_cast<decltype(c.line)>(0), c.line);
  }
  TEST(given) {
    static const char func_name[] = "hello";
    static const char file_name[] = "world";
    source_context c{func_name, file_name, 1};
    CHECK_EQUAL(func_name, c.func);
    CHECK_EQUAL(file_name, c.file);
    CHECK_EQUAL(static_cast<decltype(c.line)>(1), c.line);
  }
  TEST(macro) {
#if defined(__clang__) || defined(__GNUC__)
    const char * expected_func = __PRETTY_FUNCTION__;
#else
    const char * expected_func = __func__;
#endif
    const char * expected_file = __FILE__;
    unsigned expected_line = __LINE__ + 1;
    source_context c = SOURCE_CONTEXT;
    CHECK_EQUAL(expected_func, c.func);
    CHECK_EQUAL(expected_file, c.file);
    CHECK_EQUAL(expected_line, c.line);
  }
}

SUITE(context) {
  static const char func_name[] = "throwing_func";
  static const char file_name[] = "filename.cpp";
  static const unsigned file_line = 1234;

  TEST(null) {
    source_context c{nullptr, nullptr, 0};
    CHECK_EQUAL(string(), c.context());
  }
  TEST(a) {
    source_context c{func_name, nullptr, 0};
    CHECK_EQUAL(string("throwing_func"), c.context());
  }
  TEST(b) {
    source_context c{nullptr, file_name, 0};
    CHECK_EQUAL(string("filename.cpp"), c.context());
  }
  TEST(c) {
    source_context c{nullptr, nullptr, file_line};
    CHECK_EQUAL(string(":1234"), c.context());
  }
  TEST(ab) {
    source_context c{func_name, file_name, 0};
    CHECK_EQUAL(string("throwing_func @ filename.cpp"), c.context());
  }
  TEST(bc) {
    source_context c{nullptr, file_name, file_line};
    CHECK_EQUAL(string("filename.cpp:1234"), c.context());
  }
  TEST(ac) {
    source_context c{func_name, nullptr, file_line};
    CHECK_EQUAL(string("throwing_func:1234"), c.context());
  }
  TEST(abc) {
    source_context c{func_name, file_name, 1234u};
    CHECK_EQUAL(string("throwing_func @ filename.cpp:1234"), c.context());
  }
}
