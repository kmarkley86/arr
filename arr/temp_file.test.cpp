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

#include "arr/temp_file.hpp"
#include "arr/unistd.hpp"
#include "arr/path_exception.hpp"
#include <iostream>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;
using namespace arr;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: temp_file" << endl;
  report_operations<temp_file>(cout);
  report_operations<std::unique_ptr<char[]>>(cout);
  report_operations<wrap::file_descriptor>(cout);
}

SUITE(all) {
  TEST(full) {
    std::string name;
    wrap::file_descriptor::fd_t descriptor;
    {
      temp_file t;
      name = t.name();
      descriptor = t.get();
      cout << "Temporary file name: " << name << endl;
      cout << "Temporary file descriptor: " << descriptor << endl;
    }
    try {
      // Temporary file should have been deleted by the destructor
      wrap::unlink(SOURCE_CONTEXT, name.c_str());
      CHECK(0);
    } catch (arr::path_exception& e) {
      bool correct = e.code() == std::errc::no_such_file_or_directory;
      CHECK(correct);
      if (not correct) cerr << e.what() << '\n';
    }
  }
}
