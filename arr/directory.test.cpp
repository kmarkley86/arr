//
// Copyright (c) 2012, 2013, 2014, 2015, 2021
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

#include "arr/directory.hpp"
#include "arr/unistd.hpp"
#include "arr/file_descriptor.hpp"
#include "arr/dirent.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <type_traits>
#include <iostream>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: directory" << endl;
  arr::report_operations<wrap::directory>(cout);
}

using fd_t = wrap::file_descriptor::fd_t;

inline bool fd_valid(fd_t fd) {
  return (-1 != fcntl(fd, F_GETFL));
}

auto my_dir_p = reinterpret_cast<wrap::directory::dir_t>(0x1235);

SUITE(behavior) {

  TEST(default_const) {
    const wrap::directory d;
    CHECK_EQUAL(false, d.valid());
    CHECK_EQUAL(wrap::directory::dir_t(nullptr), d.get());
  } // Implicitly tests that the dtor does not close an invalid object

  TEST(given) {
    wrap::directory d(my_dir_p);
    CHECK_EQUAL(true, d.valid());
    CHECK_EQUAL(my_dir_p, d.get());
    CHECK_EQUAL(my_dir_p, d.release());
    CHECK_EQUAL(false, d.valid());
    CHECK_EQUAL(wrap::directory::dir_t(nullptr), d.get());
    CHECK_EQUAL(wrap::directory::dir_t(nullptr), d.release());
  }

  TEST(close_normal) {
    fd_t underlying_fd;
    {
      wrap::directory d(wrap::opendir(SOURCE_CONTEXT, "."));
      CHECK_EQUAL(true, d.valid());
      CHECK(d.get());
      underlying_fd = dirfd(d.get());
      CHECK_EQUAL(true, fd_valid(underlying_fd));
    }
    CHECK_EQUAL(false, fd_valid(underlying_fd));
  }

}

SUITE(move) {

  TEST(constructor) {
    wrap::directory f(my_dir_p);
    wrap::directory g(std::move(f));
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(wrap::directory::dir_t(nullptr), f.get());
    CHECK_EQUAL(true, g.valid());
    CHECK_EQUAL(my_dir_p, g.get());
    g.release();
  }

  TEST(assignment) {
    wrap::directory f(my_dir_p);
    wrap::directory g;
    g = std::move(f);
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(wrap::directory::dir_t(nullptr), f.get());
    CHECK_EQUAL(true, g.valid());
    CHECK_EQUAL(my_dir_p, g.get());
    g.release();
  }

  TEST(rvalue_assignment) {
    wrap::directory f;
    f = my_dir_p;
    CHECK_EQUAL(true, f.valid());
    CHECK_EQUAL(my_dir_p, f.get());
    f.release();
  }

  TEST(lvalue_assignment) {
    wrap::directory f;
    wrap::directory::dir_t x = my_dir_p;
    f = x;
    CHECK_EQUAL(true, f.valid());
    CHECK_EQUAL(my_dir_p, f.get());
    f.release();
  }

}
