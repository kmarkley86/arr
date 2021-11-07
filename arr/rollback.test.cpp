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

#include "arr/rollback.hpp"
#include <vector>
#include <iostream>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;
using namespace arr;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: rollback" << endl;
  report_operations<rollback>(cout);
}

SUITE(single) {

  TEST(committed) {
    bool commit = false;
    bool rolled_back = false;
    {
      rollback r(commit, [&rolled_back]() { rolled_back = true; });
      commit = true;
    }
    CHECK_EQUAL(false, rolled_back);
  }

  TEST(not_committed) {
    bool commit = false;
    bool rolled_back = false;
    {
      rollback r(commit, [&rolled_back]() { rolled_back = true; });
    }
    CHECK_EQUAL(true, rolled_back);
  }

  TEST(exception) {
    bool commit = false;
    bool rolled_back = false;
    try {
      rollback r(commit, [&rolled_back]() { rolled_back = true; });
      throw false;
    } catch (bool& b) {
      static_cast<void>(b);
    }
    CHECK_EQUAL(true, rolled_back);
  }

}

struct fixture {
  fixture() : rb1(false), rb2(false) { }
  bool rb1, rb2;
};

SUITE(multiple) {

  TEST_FIXTURE(throw_step0, fixture) {
    try {
      //bool commit = false;
      throw false; // step0
      //rollback r1(commit, [&]() { rb1 = true; });
      ; // step1
      //rollback r2(commit, [&]() { rb2 = true; });
      ; // step2
      //commit = true;
    } catch(bool& b) {
      static_cast<void>(b);
    }
    CHECK_EQUAL(false, rb1); // not captured in rollback yet
    CHECK_EQUAL(false, rb2); // not captured in rollback yet
  }

  TEST_FIXTURE(throw_step1, fixture) {
    try {
      bool commit = false;
      ; // step0
      rollback r1(commit, [&]() { rb1 = true; });
      throw false; // step1
      //rollback r2(commit, [&]() { rb2 = true; });
      ; // step2
      //commit = true;
    } catch(bool& b) {
      static_cast<void>(b);
    }
    CHECK_EQUAL(true, rb1);
    CHECK_EQUAL(false, rb2); // not captured in rollback yet
  }

  TEST_FIXTURE(throw_step2, fixture) {
    try {
      bool commit = false;
      ; // step0
      rollback r1(commit, [&]() { rb1 = true; });
      ; // step1
      rollback r2(commit, [&]() { rb2 = true; });
      throw false; // step2
      //commit = true;
    } catch(bool& b) {
      static_cast<void>(b);
    }
    CHECK_EQUAL(true, rb1);
    CHECK_EQUAL(true, rb2);
  }

  TEST_FIXTURE(finished, fixture) {
    try {
      bool commit = false;
      ; // step0
      rollback r1(commit, [&]() { rb1 = true; });
      ; // step1
      rollback r2(commit, [&]() { rb2 = true; });
      ; // step2
      commit = true;
    } catch(bool& b) {
      static_cast<void>(b);
    }
    CHECK_EQUAL(false, rb1);
    CHECK_EQUAL(false, rb2);
  }

}

//
// Verify that the syntax provided in the documentation's example is okay
//
namespace {
void this_might_throw() noexcept(false) { }
void this_could_throw_too() noexcept(false) { }
void some_global_cleanup_function() noexcept { }
}

SUITE(example) {
  TEST(example) {
    vector<int> my_vector;
    int my_value = 3;
    bool commit = false;
    my_vector.push_back(my_value);
    rollback r1(commit, [&] { my_vector.pop_back(); });
    this_might_throw();
    rollback r2(commit, some_global_cleanup_function);
    this_could_throw_too();
    commit = true; // There were no exceptions, so commit the operations.
  }
}
