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

#include <sstream>
#include "arrtest/arrtest.hpp"

//
// Mock directory operations using a fake filesystem
//

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
inline dirent make_dirent(const char * name, decltype(dirent::d_type) type) {
  dirent entry;
  strcpy(entry.d_name, name);
  entry.d_type = type;
  return entry;
}

#include <string>
#include <list>
#include <map>
using mock_dir = std::list<dirent>;

std::map<std::string, mock_dir> filesystem;
std::map<mock_dir*, mock_dir::iterator> iterators;

#include "arr/source_context.hpp"
namespace mock {
  namespace wrap {

    inline DIR * opendir(arr::source_context, const char * filename) {
      auto& dir = filesystem[filename];
      iterators[&dir] = dir.begin();
      return reinterpret_cast<DIR*>(&dir);
    }

    inline void closedir(arr::source_context, DIR * dirp) {
      auto p = reinterpret_cast<mock_dir*>(dirp);
      iterators.erase(p);
    }

    inline struct dirent * readdir(arr::source_context, DIR * dirp) {
      auto p = reinterpret_cast<mock_dir*>(dirp);
      auto& iter = iterators[p];
      if (iter == p->end()) {
        return nullptr;
      } else {
        auto& entry = *iter;
        ++iter;
        return &entry;
      }
    }

  }
}

//
// ----- Mock wrap::directory
//

// Includes from header
#include <sys/types.h>
#include <dirent.h>
#include <utility>
// Includes from implementation
#include "arr/dirent.hpp"

namespace mock {
#include "arr/directory.cpp"
}

//
// ----- Mock arr::directory_sequence
//

// Includes from header
#include "arr/directory.hpp"
#include "arr/syscall_exception.hpp"
#include <iterator>
#include <string>
#include <list>
#include <memory>
// Includes from implementation
#include "arr/dirent.hpp"
#include <utility>

namespace mock {
  namespace arr {
    using ::arr::syscall_exception;
    using ::arr::source_context;
  }
#include "arr/directory_sequence.cpp"
}

//
// ----- Mock arr::recursive_directory_sequence
//

// Includes from header
#include "arr/directory_sequence.hpp"
#include <deque>
// Includes from implementation
#include <utility>
#include <tuple>

namespace mock {
#include "arr/recursive_directory_sequence.cpp"
}

//
// Recursion for directories, with the directory given either before or after
// descending, makes this set of directory contents interesting:
//   {} {f} {d} {ff} {fd} {df} {dd}
//
// Test these exhaustively for two levels of depth, letting the third level
// always be empty.
//

enum test_type { E, F, D, FF, FD, DF, DD, TYPE_END };

namespace {

mock_dir create_dir(test_type type) {
  const dirent file1 = make_dirent("file1", DT_REG);
  const dirent file2 = make_dirent("file2", DT_REG);
  const dirent dir1 = make_dirent("dir1", DT_DIR);
  const dirent dir2 = make_dirent("dir2", DT_DIR);
  mock_dir dir;
  switch (type) {
    case E:
      break;
    case F:
      dir.push_back(file1);
      break;
    case D:
      dir.push_back(dir1);
      break;
    case FF:
      dir.push_back(file1);
      dir.push_back(file2);
      break;
    case FD:
      dir.push_back(file1);
      dir.push_back(dir1);
      break;
    case DF:
      dir.push_back(dir1);
      dir.push_back(file1);
      break;
    case DD:
      dir.push_back(dir1);
      dir.push_back(dir2);
      break;
    case TYPE_END:
      throw false;
  }
  return dir;
}
mock_dir create_dir(int type) {
  return create_dir(static_cast<test_type>(type));
}

void build_filesystem() {
  for (int base = 0; base < TYPE_END; ++base) {
    for (int sub1 = 0; sub1 < TYPE_END; ++sub1) {
      for (int sub2 = 0; sub2 < TYPE_END; ++sub2) {
        std::ostringstream prefix;
        prefix << "/test";
        prefix << '-' << base;
        prefix << '-' << sub1;
        prefix << '-' << sub2;
        filesystem[prefix.str()+'/'] = create_dir(base);
        switch (base) {
          case E:
          case F:
          case FF:
            goto next_base;
          case D:
          case FD:
          case DF:
            filesystem[prefix.str()+"/dir1/"] = create_dir(sub1);
            goto next_sub1;
          case DD:
            filesystem[prefix.str()+"/dir1/"] = create_dir(sub1);
            filesystem[prefix.str()+"/dir2/"] = create_dir(sub2);
            break;
          case TYPE_END:
          default:
            throw false;
        }
      }
next_sub1: ;
    }
next_base: ;
  }
}

}

using namespace std;
using mock::arr::recursive_directory_sequence;

int main(int argc, char * argv[]) {
  build_filesystem();
#ifdef ARRTEST_FILTER
  if (argc > 1) {
    auto regex = std::regex(argv[1],
        std::regex_constants::nosubs, std::regex_constants::ECMAScript);
    arr::test::tests::filter(regex);
  }
#else
  static_cast<void>(argc);
  static_cast<void>(argv);
#endif
  return arr::test::tests::run();
}

namespace {

arr::test::evaluator * global_evaluator = nullptr;

void check_end(
    mock::arr::recursive_directory_sequence& rds,
    mock::arr::recursive_directory_iterator& i)
{
  arr::test::evaluator& evaluator = *global_evaluator;
  CHECK_EQUAL(true, rds.end() == i);
}

void check_for(
    mock::arr::recursive_directory_sequence& rds,
    mock::arr::recursive_directory_iterator& i,
    const std::string& name)
{
  arr::test::evaluator& evaluator = *global_evaluator;
  CHECK_EQUAL(false, rds.end() == i);
  CHECK_EQUAL(name, rds.path()+i->d_name);
  ++i;
}

void check(
    const std::string& root,
    const std::list<std::string>& expected,
    mock::arr::recursive_directory_sequence::dir_order order
      = mock::arr::recursive_directory_sequence::dir_order::pre,
    mock::arr::recursive_directory_sequence::visit_type visit
      = mock::arr::recursive_directory_sequence::visit_type::all
      )
{
  mock::arr::recursive_directory_sequence rds(root, order, visit);
  auto i = rds.begin();
  for (auto& part : expected) {
    check_for(rds, i, root+'/'+part);
  }
  check_end(rds, i);
}

}

SUITE(normal) {

  TEST(E_x_x) {
    global_evaluator = &evaluator;
    string root = "/test-0-0-0";
    list<string> pre  = { };
    list<string> post = { };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(F_x_x) {
    global_evaluator = &evaluator;
    string root = "/test-1-0-0";
    list<string> pre  = { "file1" };
    list<string> post = { "file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(FF_x_x) {
    global_evaluator = &evaluator;
    string root = "/test-3-0-0";
    list<string> pre  = { "file1", "file2" };
    list<string> post = { "file1", "file2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(D_E_x) {
    global_evaluator = &evaluator;
    string root = "/test-2-0-0";
    list<string> pre  = { "dir1" };
    list<string> post = { "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(D_F_x) {
    global_evaluator = &evaluator;
    string root = "/test-2-1-0";
    list<string> pre  = { "dir1", "dir1/file1" };
    list<string> post = { "dir1/file1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(D_D_x) {
    global_evaluator = &evaluator;
    string root = "/test-2-2-0";
    list<string> pre  = { "dir1", "dir1/dir1" };
    list<string> post = { "dir1/dir1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(D_FF_x) {
    global_evaluator = &evaluator;
    string root = "/test-2-3-0";
    list<string> pre = { "dir1", "dir1/file1", "dir1/file2" };
    list<string> post = { "dir1/file1", "dir1/file2", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(D_FD_x) {
    global_evaluator = &evaluator;
    string root = "/test-2-4-0";
    list<string> pre  = { "dir1", "dir1/file1", "dir1/dir1" };
    list<string> post = { "dir1/file1", "dir1/dir1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(D_DF_x) {
    global_evaluator = &evaluator;
    string root = "/test-2-5-0";
    list<string> pre  = { "dir1", "dir1/dir1", "dir1/file1" };
    list<string> post = { "dir1/dir1", "dir1/file1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(D_DD_x) {
    global_evaluator = &evaluator;
    string root = "/test-2-6-0";
    list<string> pre  = { "dir1", "dir1/dir1", "dir1/dir2" };
    list<string> post = { "dir1/dir1", "dir1/dir2", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(FD_E_x) {
    global_evaluator = &evaluator;
    string root = "/test-4-0-0";
    list<string> pre  = { "file1", "dir1" };
    list<string> post = { "file1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(FD_F_x) {
    global_evaluator = &evaluator;
    string root = "/test-4-1-0";
    list<string> pre  = { "file1", "dir1", "dir1/file1" };
    list<string> post = { "file1", "dir1/file1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(FD_D_x) {
    global_evaluator = &evaluator;
    string root = "/test-4-2-0";
    list<string> pre  = { "file1", "dir1", "dir1/dir1" };
    list<string> post = { "file1", "dir1/dir1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(FD_FF_x) {
    global_evaluator = &evaluator;
    string root = "/test-4-3-0";
    list<string> pre  = { "file1", "dir1", "dir1/file1", "dir1/file2" };
    list<string> post = { "file1", "dir1/file1", "dir1/file2", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(FD_FD_x) {
    global_evaluator = &evaluator;
    string root = "/test-4-4-0";
    list<string> pre  = { "file1", "dir1", "dir1/file1", "dir1/dir1" };
    list<string> post = { "file1", "dir1/file1", "dir1/dir1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(FD_DF_x) {
    global_evaluator = &evaluator;
    string root = "/test-4-5-0";
    list<string> pre  = { "file1", "dir1", "dir1/dir1", "dir1/file1" };
    list<string> post = { "file1", "dir1/dir1", "dir1/file1", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(FD_DD_x) {
    global_evaluator = &evaluator;
    string root = "/test-4-6-0";
    list<string> pre  = { "file1", "dir1", "dir1/dir1", "dir1/dir2" };
    list<string> post = { "file1", "dir1/dir1", "dir1/dir2", "dir1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DF_E_x) {
    global_evaluator = &evaluator;
    string root = "/test-5-0-0";
    list<string> pre  = { "dir1", "file1" };
    list<string> post = { "dir1", "file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DF_F_x) {
    global_evaluator = &evaluator;
    string root = "/test-5-1-0";
    list<string> pre  = { "dir1", "dir1/file1", "file1" };
    list<string> post = { "dir1/file1", "dir1", "file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DF_D_x) {
    global_evaluator = &evaluator;
    string root = "/test-5-2-0";
    list<string> pre  = { "dir1", "dir1/dir1", "file1" };
    list<string> post = { "dir1/dir1", "dir1", "file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DF_FF_x) {
    global_evaluator = &evaluator;
    string root = "/test-5-3-0";
    list<string> pre  = { "dir1", "dir1/file1", "dir1/file2", "file1" };
    list<string> post = { "dir1/file1", "dir1/file2", "dir1", "file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DF_FD_x) {
    global_evaluator = &evaluator;
    string root = "/test-5-4-0";
    list<string> pre  = { "dir1", "dir1/file1", "dir1/dir1", "file1" };
    list<string> post = { "dir1/file1", "dir1/dir1", "dir1", "file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DF_DF_x) {
    global_evaluator = &evaluator;
    string root = "/test-5-5-0";
    list<string> pre  = { "dir1", "dir1/dir1", "dir1/file1", "file1" };
    list<string> post = { "dir1/dir1", "dir1/file1", "dir1", "file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DF_DD_x) {
    global_evaluator = &evaluator;
    string root = "/test-5-6-0";
    list<string> pre  = { "dir1", "dir1/dir1", "dir1/dir2", "file1" };
    list<string> post = { "dir1/dir1", "dir1/dir2", "dir1", "file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DD_E_E) {
    global_evaluator = &evaluator;
    string root = "/test-6-0-0";
    list<string> pre  = { "dir1", "dir2" };
    list<string> post = { "dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_E_F) {
    global_evaluator = &evaluator;
    string root = "/test-6-0-1";
    list<string> pre  = { "dir1", "dir2", "dir2/file1" };
    list<string> post = { "dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_E_D) {
    global_evaluator = &evaluator;
    string root = "/test-6-0-2";
    list<string> pre  = { "dir1", "dir2", "dir2/dir1" };
    list<string> post = { "dir1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_E_FF) {
    global_evaluator = &evaluator;
    string root = "/test-6-0-3";
    list<string> pre  = { "dir1", "dir2", "dir2/file1", "dir2/file2" };
    list<string> post = { "dir1", "dir2/file1", "dir2/file2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_E_FD) {
    global_evaluator = &evaluator;
    string root = "/test-6-0-4";
    list<string> pre  = { "dir1", "dir2", "dir2/file1", "dir2/dir1" };
    list<string> post = { "dir1", "dir2/file1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_E_DF) {
    global_evaluator = &evaluator;
    string root = "/test-6-0-5";
    list<string> pre  = { "dir1", "dir2", "dir2/dir1", "dir2/file1" };
    list<string> post = { "dir1", "dir2/dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_E_DD) {
    global_evaluator = &evaluator;
    string root = "/test-6-0-6";
    list<string> pre  = { "dir1", "dir2", "dir2/dir1", "dir2/dir2" };
    list<string> post = { "dir1", "dir2/dir1", "dir2/dir2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DD_F_E) {
    global_evaluator = &evaluator;
    string root = "/test-6-1-0";
    list<string> pre  = { "dir1", "dir1/file1", "dir2" };
    list<string> post = { "dir1/file1", "dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_F_F) {
    global_evaluator = &evaluator;
    string root = "/test-6-1-1";
    list<string> pre  = { "dir1", "dir1/file1", "dir2", "dir2/file1" };
    list<string> post = { "dir1/file1", "dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_F_D) {
    global_evaluator = &evaluator;
    string root = "/test-6-1-2";
    list<string> pre  = { "dir1", "dir1/file1", "dir2", "dir2/dir1" };
    list<string> post = { "dir1/file1", "dir1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_F_FF) {
    global_evaluator = &evaluator;
    string root = "/test-6-1-3";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir2", "dir2/file1", "dir2/file2" };
    list<string> post =
      { "dir1/file1", "dir1", "dir2/file1", "dir2/file2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_F_FD) {
    global_evaluator = &evaluator;
    string root = "/test-6-1-4";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir2", "dir2/file1", "dir2/dir1" };
    list<string> post =
      { "dir1/file1", "dir1", "dir2/file1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_F_DF) {
    global_evaluator = &evaluator;
    string root = "/test-6-1-5";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir2", "dir2/dir1", "dir2/file1" };
    list<string> post =
      { "dir1/file1", "dir1", "dir2/dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_F_DD) {
    global_evaluator = &evaluator;
    string root = "/test-6-1-6";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir2", "dir2/dir1", "dir2/dir2" };
    list<string> post =
      { "dir1/file1", "dir1", "dir2/dir1", "dir2/dir2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DD_D_E) {
    global_evaluator = &evaluator;
    string root = "/test-6-2-0";
    list<string> pre  = { "dir1", "dir1/dir1", "dir2" };
    list<string> post = { "dir1/dir1", "dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_D_F) {
    global_evaluator = &evaluator;
    string root = "/test-6-2-1";
    list<string> pre  = { "dir1", "dir1/dir1", "dir2", "dir2/file1" };
    list<string> post = { "dir1/dir1", "dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_D_D) {
    global_evaluator = &evaluator;
    string root = "/test-6-2-2";
    list<string> pre  = { "dir1", "dir1/dir1", "dir2", "dir2/dir1" };
    list<string> post = { "dir1/dir1", "dir1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_D_FF) {
    global_evaluator = &evaluator;
    string root = "/test-6-2-3";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir2", "dir2/file1", "dir2/file2" };
    list<string> post =
      { "dir1/dir1", "dir1", "dir2/file1", "dir2/file2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_D_FD) {
    global_evaluator = &evaluator;
    string root = "/test-6-2-4";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir2", "dir2/file1", "dir2/dir1" };
    list<string> post =
      { "dir1/dir1", "dir1", "dir2/file1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_D_DF) {
    global_evaluator = &evaluator;
    string root = "/test-6-2-5";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir2", "dir2/dir1", "dir2/file1" };
    list<string> post =
      { "dir1/dir1", "dir1", "dir2/dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_D_DD) {
    global_evaluator = &evaluator;
    string root = "/test-6-2-6";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir2", "dir2/dir1", "dir2/dir2" };
    list<string> post =
      { "dir1/dir1", "dir1", "dir2/dir1", "dir2/dir2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DD_FF_E) {
    global_evaluator = &evaluator;
    string root = "/test-6-3-0";
    list<string> pre  = { "dir1", "dir1/file1", "dir1/file2", "dir2" };
    list<string> post = { "dir1/file1", "dir1/file2", "dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FF_F) {
    global_evaluator = &evaluator;
    string root = "/test-6-3-1";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/file2", "dir2", "dir2/file1" };
    list<string> post =
      { "dir1/file1", "dir1/file2", "dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FF_D) {
    global_evaluator = &evaluator;
    string root = "/test-6-3-2";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/file2", "dir2", "dir2/dir1" };
    list<string> post =
      { "dir1/file1", "dir1/file2", "dir1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FF_FF) {
    global_evaluator = &evaluator;
    string root = "/test-6-3-3";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/file2", "dir2", "dir2/file1", "dir2/file2" };
    list<string> post =
      { "dir1/file1", "dir1/file2", "dir1", "dir2/file1", "dir2/file2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FF_FD) {
    global_evaluator = &evaluator;
    string root = "/test-6-3-4";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/file2", "dir2", "dir2/file1", "dir2/dir1" };
    list<string> post =
      { "dir1/file1", "dir1/file2", "dir1", "dir2/file1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FF_DF) {
    global_evaluator = &evaluator;
    string root = "/test-6-3-5";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/file2", "dir2", "dir2/dir1", "dir2/file1" };
    list<string> post =
      { "dir1/file1", "dir1/file2", "dir1", "dir2/dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FF_DD) {
    global_evaluator = &evaluator;
    string root = "/test-6-3-6";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/file2", "dir2", "dir2/dir1", "dir2/dir2" };
    list<string> post =
      { "dir1/file1", "dir1/file2", "dir1", "dir2/dir1", "dir2/dir2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DD_FD_E) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-0";
    list<string> pre  = { "dir1", "dir1/file1", "dir1/dir1", "dir2" };
    list<string> post = { "dir1/file1", "dir1/dir1", "dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FD_F) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-1";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/dir1", "dir2", "dir2/file1" };
    list<string> post =
      { "dir1/file1", "dir1/dir1", "dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FD_D) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-2";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/dir1", "dir2", "dir2/dir1" };
    list<string> post =
      { "dir1/file1", "dir1/dir1", "dir1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FD_FF) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-3";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/dir1", "dir2", "dir2/file1", "dir2/file2" };
    list<string> post =
      { "dir1/file1", "dir1/dir1", "dir1", "dir2/file1", "dir2/file2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FD_FD) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-4";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/dir1", "dir2", "dir2/file1", "dir2/dir1" };
    list<string> post =
      { "dir1/file1", "dir1/dir1", "dir1", "dir2/file1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FD_DF) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/dir1", "dir2", "dir2/dir1", "dir2/file1" };
    list<string> post =
      { "dir1/file1", "dir1/dir1", "dir1", "dir2/dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_FD_DD) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-6";
    list<string> pre  =
      { "dir1", "dir1/file1", "dir1/dir1", "dir2", "dir2/dir1", "dir2/dir2" };
    list<string> post =
      { "dir1/file1", "dir1/dir1", "dir1", "dir2/dir1", "dir2/dir2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DD_DF_E) {
    global_evaluator = &evaluator;
    string root = "/test-6-5-0";
    list<string> pre  = { "dir1", "dir1/dir1", "dir1/file1", "dir2" };
    list<string> post = { "dir1/dir1", "dir1/file1", "dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DF_F) {
    global_evaluator = &evaluator;
    string root = "/test-6-5-1";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/file1", "dir2", "dir2/file1" };
    list<string> post =
      { "dir1/dir1", "dir1/file1", "dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DF_D) {
    global_evaluator = &evaluator;
    string root = "/test-6-5-2";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/file1", "dir2", "dir2/dir1" };
    list<string> post =
      { "dir1/dir1", "dir1/file1", "dir1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DF_FF) {
    global_evaluator = &evaluator;
    string root = "/test-6-5-3";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/file1", "dir2", "dir2/file1", "dir2/file2" };
    list<string> post =
      { "dir1/dir1", "dir1/file1", "dir1", "dir2/file1", "dir2/file2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DF_FD) {
    global_evaluator = &evaluator;
    string root = "/test-6-5-4";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/file1", "dir2", "dir2/file1", "dir2/dir1" };
    list<string> post =
      { "dir1/dir1", "dir1/file1", "dir1", "dir2/file1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DF_DF) {
    global_evaluator = &evaluator;
    string root = "/test-6-5-5";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/file1", "dir2", "dir2/dir1", "dir2/file1" };
    list<string> post =
      { "dir1/dir1", "dir1/file1", "dir1", "dir2/dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DF_DD) {
    global_evaluator = &evaluator;
    string root = "/test-6-5-6";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/file1", "dir2", "dir2/dir1", "dir2/dir2" };
    list<string> post =
      { "dir1/dir1", "dir1/file1", "dir1", "dir2/dir1", "dir2/dir2", "dir2" };
    list<string> expected = { "dir1/file1" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

  TEST(DD_DD_E) {
    global_evaluator = &evaluator;
    string root = "/test-6-6-0";
    list<string> pre  = { "dir1", "dir1/dir1", "dir1/dir2", "dir2" };
    list<string> post = { "dir1/dir1", "dir1/dir2", "dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DD_F) {
    global_evaluator = &evaluator;
    string root = "/test-6-6-1";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/dir2", "dir2", "dir2/file1", };
    list<string> post =
      { "dir1/dir1", "dir1/dir2", "dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DD_D) {
    global_evaluator = &evaluator;
    string root = "/test-6-6-2";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/dir2", "dir2", "dir2/dir1" };
    list<string> post =
      { "dir1/dir1", "dir1/dir2", "dir1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DD_FF) {
    global_evaluator = &evaluator;
    string root = "/test-6-6-3";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/dir2", "dir2", "dir2/file1", "dir2/file2" };
    list<string> post =
      { "dir1/dir1", "dir1/dir2", "dir1", "dir2/file1", "dir2/file2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DD_FD) {
    global_evaluator = &evaluator;
    string root = "/test-6-6-4";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/dir2", "dir2", "dir2/file1", "dir2/dir1" };
    list<string> post =
      { "dir1/dir1", "dir1/dir2", "dir1", "dir2/file1", "dir2/dir1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DD_DF) {
    global_evaluator = &evaluator;
    string root = "/test-6-6-5";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/dir2", "dir2", "dir2/dir1", "dir2/file1" };
    list<string> post =
      { "dir1/dir1", "dir1/dir2", "dir1", "dir2/dir1", "dir2/file1", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }
  TEST(DD_DD_DD) {
    global_evaluator = &evaluator;
    string root = "/test-6-6-6";
    list<string> pre  =
      { "dir1", "dir1/dir1", "dir1/dir2", "dir2", "dir2/dir1", "dir2/dir2" };
    list<string> post =
      { "dir1/dir1", "dir1/dir2", "dir1", "dir2/dir1", "dir2/dir2", "dir2" };
    check(root, pre , mock::arr::recursive_directory_sequence::dir_order::pre);
    check(root, post, mock::arr::recursive_directory_sequence::dir_order::post);
  }

}

SUITE(visit) {
  TEST(DD_FD_DF) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    list<string> dir = { "dir1", "dir1/dir1", "dir2", "dir2/dir1" };
    list<string> reg = { "dir1/file1", "dir2/file1" };
    list<string> lnk = { };
    list<string> nd  = reg;
    auto order = mock::arr::recursive_directory_sequence::dir_order::pre;
    auto v_dir = mock::arr::recursive_directory_sequence::visit_type::directory;
    auto v_reg = mock::arr::recursive_directory_sequence::visit_type::file;
    auto v_lnk = mock::arr::recursive_directory_sequence::visit_type::link;
    auto v_nd  = mock::arr::recursive_directory_sequence::visit_type::non_dir;
    check(root, dir, order, v_dir);
    check(root, reg, order, v_reg);
    check(root, lnk, order, v_lnk);
    check(root, nd , order, v_nd );
  }
}

SUITE(abandon) {

  TEST(DD_FD_DF_pre_1) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::pre;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_for(rds, i, root+"/dir2");
    check_for(rds, i, root+"/dir2/dir1");
    check_for(rds, i, root+"/dir2/file1");
    check_end(rds, i);
  }

  TEST(DD_FD_DF_pre_2) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::pre;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir1/file1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_for(rds, i, root+"/dir2");
    check_for(rds, i, root+"/dir2/dir1");
    check_for(rds, i, root+"/dir2/file1");
    check_end(rds, i);
  }

  TEST(DD_FD_DF_pre_3) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::pre;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir1/file1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir1/dir1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_for(rds, i, root+"/dir2");
    check_for(rds, i, root+"/dir2/dir1");
    check_for(rds, i, root+"/dir2/file1");
    check_end(rds, i);
  }

  TEST(DD_FD_DF_pre_4) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::pre;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir2", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_end(rds, i);
  }

  TEST(DD_FD_DF_pre_5) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::pre;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    check_for(rds, i, root+"/dir2");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir2/dir1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_for(rds, i, root+"/dir2/file1");
    check_end(rds, i);
  }

  TEST(DD_FD_DF_pre_6) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::pre;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    check_for(rds, i, root+"/dir2");
    check_for(rds, i, root+"/dir2/dir1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir2/file1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_end(rds, i);
  }

  TEST(DD_FD_DF_pre_7) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::pre;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    check_for(rds, i, root+"/dir2");
    check_for(rds, i, root+"/dir2/dir1");
    check_for(rds, i, root+"/dir2/file1");
    CHECK_EQUAL(true, rds.end() == i);
    rds.abandon();
    ++i;
    check_end(rds, i);
  }

  TEST(DD_FD_DF_post_1) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::post;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir1/file1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir2/dir1");
    check_for(rds, i, root+"/dir2/file1");
    check_for(rds, i, root+"/dir2");
    check_end(rds, i);
  }

  TEST(DD_FD_DF_post_2) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::post;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1/file1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir1/dir1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir2/dir1");
    check_for(rds, i, root+"/dir2/file1");
    check_for(rds, i, root+"/dir2");
    check_end(rds, i);
  }

  TEST(DD_FD_DF_post_3) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::post;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_end(rds, i);
  }

  TEST(DD_FD_DF_post_4) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::post;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    check_for(rds, i, root+"/dir1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir2/dir1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_for(rds, i, root+"/dir2");
    check_end(rds, i);
  }

  TEST(DD_FD_DF_post_5) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::post;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir2/dir1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir2/file1", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_for(rds, i, root+"/dir2");
    check_end(rds, i);
  }

  TEST(DD_FD_DF_post_6) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::post;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir2/dir1");
    check_for(rds, i, root+"/dir2/file1");
    CHECK_EQUAL(false, rds.end() == i);
    CHECK_EQUAL(root+"/dir2", rds.path()+i->d_name);
    rds.abandon();
    ++i;
    check_end(rds, i);
  }

  TEST(DD_FD_DF_post_7) {
    global_evaluator = &evaluator;
    string root = "/test-6-4-5";
    auto order = mock::arr::recursive_directory_sequence::dir_order::post;
    mock::arr::recursive_directory_sequence rds(root, order);
    auto i = rds.begin();
    check_for(rds, i, root+"/dir1/file1");
    check_for(rds, i, root+"/dir1/dir1");
    check_for(rds, i, root+"/dir1");
    check_for(rds, i, root+"/dir2/dir1");
    check_for(rds, i, root+"/dir2/file1");
    check_for(rds, i, root+"/dir2");
    CHECK_EQUAL(true, rds.end() == i);
    rds.abandon();
    ++i;
    check_end(rds, i);
  }

}

SUITE(errors) {
}
