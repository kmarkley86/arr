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

#include "arr/arg_env.hpp"
#include "arr/child.hpp"
#include "arr/process_id.hpp"
#include "arr/path_exception.hpp"
#include "arr/temp_file.hpp"
#include "arr/fd_stream.hpp"
#include "arr/unistd.hpp"
#include "arr/restore.hpp"
#include <iostream>
#include <fcntl.h>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"
using namespace std;

using fd_t = wrap::file_descriptor::fd_t;

inline bool fd_valid(fd_t fd) {
  return (-1 != fcntl(fd, F_GETFL));
}

inline bool check_fd(fd_t fd, bool should_be_valid) {
  return should_be_valid == fd_valid(fd);
}

const char * program;

int main(int argc, char * argv[]) {
  program = argv[0];
  arr::arguments a(argv);
  arr::environment e;

  // Are we running as the unit test or as the child?
  if (e.end() == e.find("AS_CHILD")) {
    arr::restore<decltype(wrap::fork_retry_delay)> set_fork_retry_delay(
        wrap::fork_retry_delay, std::chrono::milliseconds(100));
    return arr::test::tests::run();
  }

  // Execution as a child process
  bool ok = true;
  if (argc > 1) {
    ok &= argv[1] == e["EXPECT_ARGUMENT"];
  }
  if (e["EXPECT_STDIN_VALID"] == "0") {
// Don't check; some environments guarantee standard file descriptors are open
//    ok &= check_fd(STDIN_FILENO, false);
  }
  if (e["EXPECT_STDIN_VALID"] == "1") {
    ok &= check_fd(STDIN_FILENO, true);
    std::string input;
    cin >> input;
    ok &= input == "stdin";
  }
  if (e["EXPECT_STDOUT_VALID"] == "0") {
// Don't check; some environments guarantee standard file descriptors are open
//    ok &= check_fd(STDOUT_FILENO, false);
  }
  if (e["EXPECT_STDOUT_VALID"] == "1") {
    ok &= check_fd(STDOUT_FILENO, true);
    cout << "stdout" << endl;
  }
  if (e["EXPECT_STDERR_VALID"] == "0") {
// Don't check; some environments guarantee standard file descriptors are open
//    ok &= check_fd(STDERR_FILENO, false);
  }
  if (e["EXPECT_STDERR_VALID"] == "1") {
    ok &= check_fd(STDERR_FILENO, true);
    cerr << "stderr" << endl;
  }
  if (ok) {
    return 0;
  } else {
    return 1;
  }
}

SUITE(usage) {

  TEST(simplest) {
    arr::arguments argv;
    arr::environment envp;
    argv.push_back(program);
    envp.insert(make_pair("AS_CHILD", "1"));
    wrap::process_id pid = arr::spawn_stdio(argv, envp);
    pid.wait();
    CHECK_EQUAL(true, pid.exited());
    CHECK_EQUAL(0, pid.exit_status());
  }

  TEST(argument) {
    arr::arguments argv;
    arr::environment envp;
    argv.push_back(program);
    argv.push_back("some_argument");
    envp.insert(make_pair("AS_CHILD", "1"));
    envp.insert(make_pair("EXPECT_ARGUMENT", "some_argument"));
    wrap::process_id pid = arr::spawn_stdio(argv, envp);
    pid.wait();
    CHECK_EQUAL(true, pid.exited());
    CHECK_EQUAL(0, pid.exit_status());
  }

  TEST(all_invalid) {
    arr::arguments argv;
    arr::environment envp;
    argv.push_back(program);
    envp.insert(make_pair("AS_CHILD", "1"));
    envp.insert(make_pair("EXPECT_STDIN_VALID", "0"));
    envp.insert(make_pair("EXPECT_STDOUT_VALID", "0"));
    envp.insert(make_pair("EXPECT_STDERR_VALID", "0"));
    wrap::process_id pid = arr::spawn_stdio(argv, envp, -1, -1, -1);
    pid.wait();
    CHECK_EQUAL(true, pid.exited());
    CHECK_EQUAL(0, pid.exit_status());
  }

  TEST(all_valid) {
    arr::arguments argv;
    arr::environment envp;
    argv.push_back(program);
    envp.insert(make_pair("AS_CHILD", "1"));
    envp.insert(make_pair("EXPECT_STDIN_VALID", "1"));
    envp.insert(make_pair("EXPECT_STDOUT_VALID", "1"));
    envp.insert(make_pair("EXPECT_STDERR_VALID", "1"));
    arr::temp_file for_in;
    arr::temp_file for_out;
    arr::temp_file for_err;
    fd_t in_d = for_in.get();
    fd_t out_d = for_out.get();
    fd_t err_d = for_err.get();
    arr::fd_ostream stream_in(for_in);
    arr::fd_istream stream_out(for_out);
    arr::fd_istream stream_err(for_err);
    stream_in << "stdin" << endl;
    wrap::lseek(SOURCE_CONTEXT, in_d, 0, SEEK_SET);
    wrap::process_id pid = arr::spawn_stdio(argv, envp, in_d, out_d, err_d);
    pid.wait();
    wrap::lseek(SOURCE_CONTEXT, out_d, 0, SEEK_SET);
    wrap::lseek(SOURCE_CONTEXT, err_d, 0, SEEK_SET);
    CHECK_EQUAL(true, pid.exited());
    CHECK_EQUAL(0, pid.exit_status());
    std::string temp;
    stream_out >> temp;
    CHECK_EQUAL("stdout", temp);
    stream_err >> temp;
    CHECK_EQUAL("stderr", temp);
  }

  TEST(mingle_stdout_stderr) {
    arr::arguments argv;
    arr::environment envp;
    argv.push_back(program);
    envp.insert(make_pair("AS_CHILD", "1"));
    envp.insert(make_pair("EXPECT_STDIN_VALID", "0"));
    envp.insert(make_pair("EXPECT_STDOUT_VALID", "1"));
    envp.insert(make_pair("EXPECT_STDERR_VALID", "1"));
    arr::temp_file for_both;
    fd_t both_d = for_both.get();
    arr::fd_istream stream_both(for_both);
    wrap::process_id pid = arr::spawn_stdio(argv, envp, -1, both_d, both_d);
    pid.wait();
    wrap::lseek(SOURCE_CONTEXT, both_d, 0, SEEK_SET);
    CHECK_EQUAL(true, pid.exited());
    CHECK_EQUAL(0, pid.exit_status());
    std::string temp;
    stream_both >> temp;
    CHECK_EQUAL("stdout", temp);
    stream_both >> temp;
    CHECK_EQUAL("stderr", temp);
  }

}

SUITE(errors) {

  TEST(not_executable) {
    arr::arguments argv;
    arr::temp_file file; // Not executable
    argv.push_back(file.name());
    try {
      arr::spawn_stdio(argv);
      CHECK(0);
    } catch (arr::path_exception& e) {
      bool correct = e.code() == std::errc::permission_denied;
      CHECK(correct);
      if (not correct) cerr << e.what() << '\n';
    }
  }

  TEST(a_directory) {
    arr::arguments argv;
    argv.push_back(".");
    try {
      arr::spawn_stdio(argv);
      CHECK(0);
    } catch (arr::path_exception& e) {
      bool correct = e.code() == std::errc::is_a_directory;
      CHECK(correct);
      if (not correct) cerr << e.what() << '\n';
    }
  }

  TEST(does_not_exist) {
    arr::arguments argv;
    {
      arr::temp_file file;
      argv.push_back(file.name());
    } // Delete temporary file
    try {
      arr::spawn_stdio(argv);
      CHECK(0);
    } catch (arr::path_exception& e) {
      bool correct = e.code() == std::errc::no_such_file_or_directory;
      CHECK(correct);
      if (not correct) cerr << e.what() << '\n';
    }
  }

}

SUITE(channels) {

  TEST(no_channel) {
    arr::no_channel c;
    CHECK_EQUAL(false, c.parent().valid());
    CHECK_EQUAL(false, c. child().valid());
  }

  TEST(fd_channel) {
    // Steal the file descriptor from a temporary file
    arr::temp_file file;
    arr::fd_channel c(file);
    CHECK_EQUAL(false, file.valid());
    CHECK_EQUAL(false, c.parent().valid());
    CHECK_EQUAL( true, c. child().valid());
  }

  TEST(read_pipe) {
    arr::arguments argv;
    arr::environment envp;
    argv.push_back(program);
    envp.insert(make_pair("AS_CHILD", "1"));
    envp.insert(make_pair("EXPECT_STDIN_VALID", "0"));
    envp.insert(make_pair("EXPECT_STDOUT_VALID", "1"));
    envp.insert(make_pair("EXPECT_STDERR_VALID", "1"));
    arr::read_pipe c;
    CHECK_EQUAL( true, c.parent().valid());
    CHECK_EQUAL( true, c. child().valid());
    wrap::process_id pid = arr::spawn_stdio(argv, envp,
        -1, c.child().get(), c.child().get());
    CHECK_EQUAL( true, c.parent().valid());
    CHECK_EQUAL( true, c. child().valid());
    c.close_child();
    CHECK_EQUAL( true, c.parent().valid());
    CHECK_EQUAL(false, c. child().valid());
    arr::fd_istream in(c.parent());
    CHECK_EQUAL(false, c.parent().valid());
    CHECK_EQUAL(false, c. child().valid());
    std::string temp;
    in >> temp;
    CHECK_EQUAL("stdout", temp);
    in >> temp;
    CHECK_EQUAL("stderr", temp);
    pid.wait();
    CHECK_EQUAL(true, pid.exited());
    CHECK_EQUAL(0, pid.exit_status());
  }

  TEST(write_pipe) {
    arr::arguments argv;
    arr::environment envp;
    argv.push_back(program);
    envp.insert(make_pair("AS_CHILD", "1"));
    envp.insert(make_pair("EXPECT_STDIN_VALID", "1"));
    envp.insert(make_pair("EXPECT_STDOUT_VALID", "0"));
    envp.insert(make_pair("EXPECT_STDERR_VALID", "0"));
    arr::write_pipe c;
    CHECK_EQUAL( true, c.parent().valid());
    CHECK_EQUAL( true, c. child().valid());
    wrap::process_id pid = arr::spawn_stdio(argv, envp,
        c.child().get(), -1, -1);
    CHECK_EQUAL( true, c.parent().valid());
    CHECK_EQUAL( true, c. child().valid());
    c.close_child();
    CHECK_EQUAL( true, c.parent().valid());
    CHECK_EQUAL(false, c. child().valid());
    arr::fd_ostream out(c.parent());
    out << "stdin" << endl;
    pid.wait();
    CHECK_EQUAL(true, pid.exited());
    CHECK_EQUAL(0, pid.exit_status());
  }

  TEST(bidirectional_pipe) {
    arr::arguments argv;
    arr::environment envp;
    argv.push_back(program);
    envp.insert(make_pair("AS_CHILD", "1"));
    envp.insert(make_pair("EXPECT_STDIN_VALID", "1"));
    envp.insert(make_pair("EXPECT_STDOUT_VALID", "1"));
    envp.insert(make_pair("EXPECT_STDERR_VALID", "0"));
    arr::read_pipe r;
    arr::write_pipe w;
    wrap::process_id pid = arr::spawn_stdio(argv, envp,
        w.child().get(), r.child().get(), -1);
    r.close_child();
    w.close_child();
    arr::fd_ostream out(w.parent());
    arr::fd_istream  in(r.parent());
    out << "stdin" << endl;
    std::string temp;
    in >> temp;
    CHECK_EQUAL("stdout", temp);
    pid.wait();
    CHECK_EQUAL(true, pid.exited());
    CHECK_EQUAL(0, pid.exit_status());
  }

}
