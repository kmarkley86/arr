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

#include "arr/errno_exception.hpp"
#include "arr/arg_env.hpp"
#include "arr/execute.hpp"
#include "arr/temp_file.hpp"
#include "arr/report.test.hpp"
#include "arr/restore.hpp"
#include "arrtest/arrtest.hpp"
#include <fstream>
#include <cstdlib>
using namespace std;

arr::arguments args;

int main(int, char * argv[]) {
  args.push_back(argv[0]);
  arr::arguments a(argv);
  arr::environment e;

  // Are we running as the unit test or as the child?
  if (e.end() == e.find("AS_CHILD")) {
    setenv("AS_CHILD", "1", 1);
    cout << "Operations of type: execute\n";
    arr::report_operations<arr::execute>(cout);
    cout << "Operations of type: execute_capture\n";
    arr::report_operations<arr::execute_capture>(cout);
    cout << "Operations of type: execute_log_fd\n";
    arr::report_operations<arr::execute_log_fd>(cout);
    cout << "Operations of type: execute_log_file\n";
    arr::report_operations<arr::execute_log_file>(cout);
    cout << "Operations of type: execute_io\n";
    arr::report_operations<arr::execute_io>(cout);
    arr::restore<decltype(wrap::fork_retry_delay)> set_fork_retry_delay(
        wrap::fork_retry_delay, std::chrono::milliseconds(100));
    return arr::test::tests::run();
  }

  std::cin .exceptions(std::istream::failbit | std::ifstream::badbit);
  std::cout.exceptions(std::istream::failbit | std::ifstream::badbit);
  try {
    if (e["READ_INPUT"] == "1") {
      std::string line;
      std::getline(std::cin, line);
      // Add a '*' at the beginning of the line
      std::cout << "*" << line << std::endl;
    }
    std::cout << "child exiting" << std::endl;
  } catch (std::system_error& error) {
    std::cerr << error.what();
    arr::errno_exception error2;
    std::cerr << error2.what();
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

SUITE(objects) {

  TEST(execute) {
    arr::execute child(args);
    child.wait();
    CHECK_EQUAL(true, child.successful());
  }

  TEST(execute_capture) {
    arr::execute_capture child(args);
    std::string line;
    std::getline(child, line);
    CHECK_EQUAL("child exiting", line);
    child.wait();
    CHECK_EQUAL(true, child.successful());
  }

  TEST(execute_log_fd) {
    arr::temp_file tf;
    arr::fd_ostream os(tf);
    os << "parent before" << endl;
    arr::execute_log_fd child(args, os.fd());
    child.wait();
    CHECK_EQUAL(true, child.successful());
    os << "parent after" << endl;
    std::ifstream logfile(tf.name());
    std::string line;
    std::getline(logfile, line);
    CHECK_EQUAL("parent before", line);
    std::getline(logfile, line);
    CHECK_EQUAL("child exiting", line);
    std::getline(logfile, line);
    CHECK_EQUAL("parent after", line);
  }

  TEST(execute_log_file) {
    arr::temp_file tf;
    arr::execute_log_file child(args, tf.name(), O_WRONLY, 0600);
    child.wait();
    CHECK_EQUAL(true, child.successful());
    arr::fd_istream logfile(tf);
    std::string line;
    std::getline(logfile, line);
    CHECK_EQUAL("child exiting", line);
  }

  TEST(execute_io) {
    setenv("READ_INPUT", "1", 1);
    arr::execute_io child(args);
    std::string line;
    child << "Hello, child!";
    child << std::endl;
    std::getline(child.read, line);
    CHECK_EQUAL("*Hello, child!", line);
    std::getline(child.read, line);
    CHECK_EQUAL("child exiting", line);
    child.wait();
    CHECK_EQUAL(true, child.successful());
  }

}
