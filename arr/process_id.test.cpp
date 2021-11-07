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

#include "arr/process_id.hpp"
#include "arr/child.hpp"
#include "arr/arg_env.hpp"
#include "arr/syscall_exception.hpp"
#include "arr/temp_dir.hpp"
#include "arr/restore.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include "arr/report.test.hpp"
#include "arrtest/arrtest.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;

const char * program;

int main(int argc, char * argv[]) {
  // Facilitate testing results of processes
  if (argc >= 3) {
    int signal = std::stoi(argv[1]);
    int status = std::stoi(argv[2]);
    const char * dir = argv[3];
    if (dir) wrap::chdir(SOURCE_CONTEXT, dir);
    sleep(1);
    if (signal) kill(getpid(), signal);
    exit(status);
  }

  program = argv[0];
  cout << "Operations of type: process_id" << endl;
  arr::report_operations<wrap::process_id>(cout);
  arr::restore<decltype(wrap::fork_retry_delay)> set_fork_retry_delay(
      wrap::fork_retry_delay, std::chrono::milliseconds(100));
  return arr::test::tests::run();
}

using pid_et = wrap::process_id::pid_et;
const pid_t invalid = static_cast<pid_t>(pid_et::invalid);

SUITE(behavior) {

  TEST(default_const) {
    wrap::process_id p;
    CHECK_EQUAL(false, p.valid());
    CHECK_EQUAL(invalid, p.get());
  } // Implicitly tests that the dtor does not close an invalid pid

  TEST(success_exit) {
    arr::arguments a;
    a.push_back(program);
    a.push_back("0");
    a.push_back("0");
    wrap::process_id p = arr::spawn_stdio(a);
    CHECK_EQUAL(true, p.valid());
    CHECK_EQUAL(false, p.completed());
    p.wait();
    CHECK_EQUAL(true , p.completed());
    CHECK_EQUAL(true , p.exited());
    CHECK_EQUAL(false, p.signaled());
    CHECK_EQUAL(false, p.coredump());
    CHECK_EQUAL(0, p.exit_status());
    CHECK_EQUAL(true, p.successful());
    CHECK_EQUAL("exited with status 0", p.result());
  }

  TEST(fail_exit) {
    arr::arguments a;
    a.push_back(program);
    a.push_back("0");
    a.push_back("2");
    wrap::process_id p = arr::spawn_stdio(a);
    CHECK_EQUAL(true, p.valid());
    CHECK_EQUAL(false, p.completed());
    p.wait();
    CHECK_EQUAL(true , p.completed());
    CHECK_EQUAL(true , p.exited());
    CHECK_EQUAL(false, p.signaled());
    CHECK_EQUAL(false, p.coredump());
    CHECK_EQUAL(2, p.exit_status());
    CHECK_EQUAL(false, p.successful());
    CHECK_EQUAL("exited with status 2", p.result());
  }

  TEST(signal_exit) {
    arr::arguments a;
    a.push_back(program);
    a.push_back(std::to_string(SIGTERM));
    a.push_back("2");
    wrap::process_id p = arr::spawn_stdio(a);
    CHECK_EQUAL(true, p.valid());
    CHECK_EQUAL(false, p.completed());
    p.wait();
    CHECK_EQUAL(true , p.completed());
    CHECK_EQUAL(false, p.exited());
    CHECK_EQUAL(true , p.signaled());
    CHECK_EQUAL(false, p.coredump());
    CHECK_EQUAL(SIGTERM, p.term_signal());
    CHECK_EQUAL(false, p.successful());
    ostringstream x;
    x << "terminated by signal " << SIGTERM;
    CHECK_EQUAL(x.str(), p.result());
  }

  TEST(core_dump) {
    struct rlimit rlim;
    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;
    if (-1 == setrlimit(RLIMIT_CORE, &rlim)) {
      throw arr::syscall_exception("setrlimit");
    }
    arr::arguments a;
    a.push_back(program);
    a.push_back(std::to_string(SIGQUIT));
    a.push_back("2");
    // Pass a temp directory name so the core file can be cleaned up
    arr::temp_dir dir;
    a.push_back(dir.name());
    wrap::process_id p = arr::spawn_stdio(a);
    CHECK_EQUAL(true, p.valid());
    CHECK_EQUAL(false, p.completed());
    p.wait();
    CHECK_EQUAL(true , p.completed());
    CHECK_EQUAL(false, p.exited());
    CHECK_EQUAL(true , p.signaled());
    CHECK_EQUAL(true , p.coredump());
    CHECK_EQUAL(SIGQUIT, p.term_signal());
    CHECK_EQUAL(false, p.successful());
    ostringstream x;
    x << "terminated by signal " << SIGQUIT << ", core dumped";
    CHECK_EQUAL(x.str(), p.result());
  }

  TEST(dtor_wait) {
    arr::arguments a;
    a.push_back(program);
    a.push_back("0");
    a.push_back("0");
    wrap::process_id copy;
    {
      wrap::process_id p = arr::spawn_stdio(a);
      copy = p.get();
      CHECK_EQUAL(true, p.valid());
      CHECK_EQUAL(false, p.completed());
    } // implicit wait, so another wait() should fail
    try {
      copy.wait();
      CHECK_CATCH(arr::syscall_exception, e);
      static_cast<void>(e);
    }
    copy.release();
  }

}

SUITE(move) {

  TEST(constructor) {
    wrap::process_id f(1);
    wrap::process_id g(std::move(f));
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(invalid, f.get());
    CHECK_EQUAL(true, g.valid());
    CHECK_EQUAL(1, g.get());
    g.release();
  }

  TEST(assignment) {
    wrap::process_id f(1);
    wrap::process_id g;
    g = std::move(f);
    CHECK_EQUAL(false, f.valid());
    CHECK_EQUAL(invalid, f.get());
    CHECK_EQUAL(true, g.valid());
    CHECK_EQUAL(1, g.get());
    g.release();
  }

  TEST(rvalue_assignment) {
    wrap::process_id f;
    f = 1;
    CHECK_EQUAL(true, f.valid());
    CHECK_EQUAL(1, f.get());
    f.release();
  }

  TEST(lvalue_assignment) {
    wrap::process_id f;
    pid_t x = 1;
    f = x;
    CHECK_EQUAL(true, f.valid());
    CHECK_EQUAL(1, f.get());
    f.release();
  }

}
