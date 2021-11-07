//
// Copyright (c) 2012, 2013, 2016, 2021
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

#include "arr/glob.hpp"
#include "arr/file_descriptor.hpp"
#include "arr/temp_dir.hpp"
#include "arr/fcntl.hpp"
#include "arrtest/arrtest.hpp"
using namespace std;
using namespace arr;

UNIT_TEST_MAIN

inline void create_file(const char * dir, string name) {
  wrap::file_descriptor fd = wrap::open(SOURCE_CONTEXT,
      (dir + name).c_str(), O_WRONLY|O_CREAT, 0600);
}

SUITE(all) {

  TEST(no_wild) {
    temp_dir d;
    auto results = wrap::glob(SOURCE_CONTEXT, d.name());
    CHECK_EQUAL(1u, results.size());
    CHECK_EQUAL(d.name(), results.front());
  }

  TEST(matches) {
    temp_dir d;
    create_file(d.name(), "/aa");
    create_file(d.name(), "/ba");
    create_file(d.name(), "/bb");
    string glob = d.name() + string("/*a");
    auto results = wrap::glob(SOURCE_CONTEXT, glob.c_str());
    CHECK_EQUAL(2u, results.size());
  }

  TEST(no_matches) {
    temp_dir d;
    create_file(d.name(), "/aa");
    create_file(d.name(), "/ba");
    create_file(d.name(), "/bb");
    string glob = d.name() + string("/*x");
    auto results = wrap::glob(SOURCE_CONTEXT, glob.c_str());
    CHECK_EQUAL(0u, results.size());
  }

}
