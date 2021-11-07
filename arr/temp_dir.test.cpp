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

#include "arr/temp_dir.hpp"
#include "arr/directory.hpp"
#include "arr/fcntl.hpp"
#include "arr/unistd.hpp"
#include "arr/dirent.hpp"
#include "arr/path_exception.hpp"
#include <iostream>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"

using namespace std;
using namespace arr;

UNIT_TEST_MAIN

TEST(report) {
  cout << "Operations of type: temp_dir" << endl;
  report_operations<temp_dir>(cout);
  report_operations<std::unique_ptr<char[]>>(cout);
}

SUITE(all) {

  TEST(empty) {
    std::string name;
    {
      temp_dir t;
      name = t.name();
      cout << "Temporary dir name: " << name << endl;
      // opendir should succeed
      wrap::directory d = wrap::opendir(SOURCE_CONTEXT, name.c_str());
    }
    try {
      // Temporary dir should have been deleted by the destructor
      wrap::rmdir(SOURCE_CONTEXT, name.c_str());
      CHECK(0);
    } catch (arr::path_exception& e) {
      bool correct = e.code() == std::errc::no_such_file_or_directory;
      CHECK(correct);
      if (not correct) cerr << e.what() << '\n';
    }
  }

  TEST(nonempty) {
    std::string name;
    {
      temp_dir t;
      name = t.name();
      cout << "Temporary dir name: " << name << endl;
      std::string a_subdir = name+"/subdir";
      std::string a_file   = name+"/subdir/file";
      wrap::mkdir(SOURCE_CONTEXT, a_subdir.c_str(), 0700);
      wrap::open (SOURCE_CONTEXT, a_file.c_str(), O_RDWR|O_CREAT, 0600);
    }
    try {
      // Temporary dir should have been deleted by the destructor
      wrap::rmdir(SOURCE_CONTEXT, name.c_str());
      CHECK(0);
    } catch (arr::path_exception& e) {
      bool correct = e.code() == std::errc::no_such_file_or_directory;
      CHECK(correct);
      if (not correct) cerr << e.what() << '\n';
    }
  }

}
