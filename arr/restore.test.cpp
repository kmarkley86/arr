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

#include "arr/restore.hpp"
#include <iostream>
#include <cerrno>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;
using namespace arr;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: restore" << endl;
  report_operations<restore<bool>>(cout);
  report_operations<restore<string>>(cout);
}

SUITE(one) {

  TEST(normal) {
    bool a_variable = false;
    {
      restore<decltype(a_variable)> restorer(a_variable);
      a_variable = true;
    }
    CHECK_EQUAL(false, a_variable);
  }

  TEST(func) {
    bool a_variable = false;
    {
      auto x = make_restore(a_variable);
      a_variable = true;
    }
    CHECK_EQUAL(false, a_variable);
  }

  TEST(ref) {
    errno = 0;
    {
      auto x = make_restore(errno);
      errno = 1;
    }
    CHECK_EQUAL(0, errno);
  }

}

SUITE(two) {

  TEST(normal) {
    bool a_variable = false;
    {
      restore<decltype(a_variable)> restorer(a_variable, true);
      CHECK_EQUAL(true, a_variable);
    }
    CHECK_EQUAL(false, a_variable);
  }

  TEST(func) {
    bool a_variable = false;
    {
      auto x = make_restore(a_variable, true);
      CHECK_EQUAL(true, a_variable);
    }
    CHECK_EQUAL(false, a_variable);
  }

  TEST(ref) {
    errno = 0;
    {
      auto x = make_restore(errno, 1);
      CHECK_EQUAL(1, errno);
    }
    CHECK_EQUAL(0, errno);
  }

}
