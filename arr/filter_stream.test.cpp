//
// Copyright (c) 2012, 2013, 2014, 2021
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

#include "arr/filter_stream.hpp"
#include "arr/temp_file.hpp"
#include "arr/fcntl.hpp"
#include "arr/unistd.hpp"
#include "arr/restore.hpp"
#include "arrtest/arrtest.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;
using namespace wrap;

using fd_t = file_descriptor::fd_t;

arr::arguments args;

int main(int, char * argv[]) {
  args.push_back(argv[0]);
  arr::arguments a(argv);
  arr::environment e;

  // Are we running as the unit test or as the child?
  if (e.end() == e.find("AS_CHILD")) {
    setenv("AS_CHILD", "1", 1);
    arr::restore<decltype(wrap::fork_retry_delay)> set_fork_retry_delay(
        wrap::fork_retry_delay, std::chrono::milliseconds(100));
    return arr::test::tests::run();
  }

  // Execution as a child process
  std::string line;
  while (std::getline(std::cin, line)) {
    // Add a '*' at the beginning of each line
    std::cout << "*" << line << "\n";
  }
  return 0;
}

SUITE(free_functions) {

  TEST(input) {
    arr::temp_file temp;
    {
      arr::fd_ostream out(temp);
      out << "Line 1\nLine 2\n";
    }
    file_descriptor id = open(SOURCE_CONTEXT, temp.name(), O_RDONLY, 0);
    process_id p = arr::ifilter(id, args);
    arr::fd_istream in(id);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("*Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("*Line 2", line);
  }

  TEST(output) {
    arr::temp_file temp;
    {
      process_id p = arr::ofilter(temp, args);
      arr::fd_ostream out(temp);
      out << "Line 1\nLine 2\n";
    }
    file_descriptor id = open(SOURCE_CONTEXT, temp.name(), O_RDONLY, 0);
    arr::fd_istream in(id);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("*Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("*Line 2", line);
  }

}

SUITE(direct) {

  TEST(input) {
    arr::temp_file temp;
    {
      arr::fd_ostream out(temp);
      out << "Line 1\nLine 2\n";
    }
    arr::filter_ifstream in(args, temp.name(), O_RDONLY);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("*Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("*Line 2", line);
  }

  TEST(outptut) {
    arr::temp_file temp;
    {
      arr::filter_ofstream out(args, temp.name(), O_WRONLY, 0600);
      out << "Line 1\nLine 2\n";
    }
    arr::fd_istream in(temp);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("*Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("*Line 2", line);
  }

}

SUITE(search_miss) {

  TEST(input) {
    arr::temp_file temp;
    {
      arr::fd_ostream out(temp);
      out << "Line 1\nLine 2\n";
    }
    arr::filter_ifstream in(temp.name(), O_RDONLY);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("Line 2", line);
  }

  TEST(output) {
    arr::temp_file temp;
    {
      arr::filter_ofstream out(temp.name(), O_WRONLY, 0600);
      out << "Line 1\nLine 2\n";
    }
    arr::fd_istream in(temp);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("Line 2", line);
  }

}

SUITE(search_hit) {

  TEST(input) {
    arr::filter_ifstream::add_filter(std::regex(".*"), args);
    arr::temp_file temp;
    {
      arr::fd_ostream out(temp);
      out << "Line 1\nLine 2\n";
    }
    arr::filter_ifstream in(temp.name(), O_RDONLY);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("*Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("*Line 2", line);
  }

  TEST(output) {
    arr::filter_ofstream::add_filter(std::regex(".*"), args);
    arr::temp_file temp;
    {
      arr::filter_ofstream out(temp.name(), O_WRONLY, 0600);
      out << "Line 1\nLine 2\n";
    }
    arr::fd_istream in(temp);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("*Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("*Line 2", line);
  }

}

SUITE(compound) {

  TEST(compound) {
    arr::temp_file temp;
    {
      arr::filter_ofstream out(temp.name(), O_WRONLY, 0600);
      out << "Line 1\nLine 2\n";
    }
    arr::filter_ifstream in(temp.name(), O_RDONLY);
    std::string line;
    std::getline(in, line);
    CHECK_EQUAL("**Line 1", line);
    std::getline(in, line);
    CHECK_EQUAL("**Line 2", line);
  }

}
