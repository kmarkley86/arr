//
// Copyright (c) 2012, 2013, 2015, 2016, 2021
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

#include "arr/file_descriptor.hpp"
#include "arr/syscall_exception.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: file_descriptor" << endl;
  arr::report_operations<wrap::file_descriptor>(cout);
}

using fd_t = wrap::file_descriptor::fd_t;
using fd_et = wrap::file_descriptor::fd_et;
const fd_t invalid = static_cast<fd_t>(fd_et::invalid);

inline bool fd_valid(fd_t fd) {
  return (-1 != fcntl(fd, F_GETFL));
}

SUITE(behavior) {

  TEST(default_const) {
    const wrap::file_descriptor f;
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(invalid, f.get());
  } // Implicitly tests that the dtor does not close an invalid descriptor

  TEST(given) {
    CHECK_EQUAL(true, fd_valid(STDOUT_FILENO));
    {
      wrap::file_descriptor f(STDOUT_FILENO);
      CHECK_EQUAL(true, f.valid());
      CHECK_EQUAL(STDOUT_FILENO, f.get());
      CHECK_EQUAL(STDOUT_FILENO, f.release());
      CHECK_EQUAL(false, f.valid());
      CHECK_EQUAL(invalid, f.get());
      CHECK_EQUAL(invalid, f.release());
    }
    CHECK_EQUAL(true, fd_valid(STDOUT_FILENO));
  }

  TEST(close_normal) {
    CHECK_EQUAL(true, fd_valid(STDIN_FILENO));
    wrap::file_descriptor f(STDIN_FILENO);
    CHECK_EQUAL(true, f.valid());
    CHECK_EQUAL(STDIN_FILENO, f.get());
    f.close();
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(invalid, f.get());
    CHECK_EQUAL(false, fd_valid(STDIN_FILENO));
  }

  TEST(close_error) {
    CHECK_EQUAL(false, fd_valid(STDIN_FILENO));
    wrap::file_descriptor f(STDIN_FILENO);
    CHECK_EQUAL(true, f.valid());
    CHECK_EQUAL(STDIN_FILENO, f.get());
    try {
      f.close();
      CHECK_CATCH(arr::syscall_exception, e);
      static_cast<void>(e);
    }
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(invalid, f.get());
    CHECK_EQUAL(false, fd_valid(STDIN_FILENO));
  }

  TEST(dtor_closes_valid) {
    // Test that the destructor closes a descriptor it belives is valid
    CHECK_EQUAL(false, fd_valid(STDIN_FILENO));
    CHECK_EQUAL(false, fd_valid(STDIN_FILENO));
  }

}

SUITE(move) {

  TEST(constructor) {
    wrap::file_descriptor f(STDIN_FILENO);
    wrap::file_descriptor g(std::move(f));
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(invalid, f.get());
    CHECK_EQUAL(true, g.valid());
    CHECK_EQUAL(STDIN_FILENO, g.get());
    g.release();
  }

  TEST(assignment) {
    wrap::file_descriptor f(STDIN_FILENO);
    wrap::file_descriptor g;
    g = std::move(f);
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(invalid, f.get());
    CHECK_EQUAL(true, g.valid());
    CHECK_EQUAL(STDIN_FILENO, g.get());
    g.release();
  }

  TEST(rvalue_assignment) {
    wrap::file_descriptor f;
    f = STDIN_FILENO;
    CHECK_EQUAL(true, f.valid());
    CHECK_EQUAL(STDIN_FILENO, f.get());
    f.release();
  }

  TEST(lvalue_assignment) {
    wrap::file_descriptor f;
    fd_t x = STDIN_FILENO;
    f = x;
    CHECK_EQUAL(true, f.valid());
    CHECK_EQUAL(STDIN_FILENO, f.get());
    f.release();
  }

}
