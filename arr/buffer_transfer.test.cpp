//
// Copyright (c) 2013, 2021, 2023
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

#include "arrtest/arrtest.hpp"
#include "arr/buffer_base.hpp"
#include "arr/buffer_transfer.hpp"
#include <sstream>
#include <string>
#include <array>

UNIT_TEST_MAIN

using enum arr::wake_policy;

SUITE(trivial) {

  TEST(create_destroy) {
    using base_t = arr::buffer_base<char>;
    base_t b(2);
    arr::buffer_direction<base_t::size_type> d;
    arr::buffer_transfer<base_t> t(b, d, all);
  }

  TEST(copy) {
    using base_t = arr::buffer_base<char>;
    base_t b(20);
    arr::buffer_direction<base_t::size_type> d;
    {
      arr::buffer_transfer<base_t> t(b, d, all);
      std::string s("Hello, world!");
      t.write(s.c_str(), 10);
    }
    CHECK_EQUAL(base_t::size_type(10), d.recent());
    CHECK_EQUAL('H', b.elements[0]);
    CHECK_EQUAL('e', b.elements[1]);
  }

  TEST(move) {
    using base_t = arr::buffer_base<char>;
    base_t b(20);
    arr::buffer_direction<base_t::size_type> d;
    {
      arr::buffer_transfer<base_t> t(b, d, all);
      std::string s("Hello, world!");
      t.write(std::make_move_iterator(s.c_str()), 10);
    }
    CHECK_EQUAL(base_t::size_type(10), d.recent());
    CHECK_EQUAL('H', b.elements[0]);
    CHECK_EQUAL('e', b.elements[1]);
  }

  TEST(read) {
    using base_t = arr::buffer_base<char>;
    base_t b(20);
    arr::buffer_direction<base_t::size_type> w;
    arr::buffer_direction<base_t::size_type> r;
    char result[20];
    {
      arr::buffer_transfer<base_t> t(b, w, all);
      std::string s("Hello, world!");
      t.write(s.c_str(), 10);
    }
    {
      arr::buffer_transfer<base_t> t(b, r, all);
      t.read(result, 5);
    }
    CHECK_EQUAL(base_t::size_type(10), w.recent());
    CHECK_EQUAL(base_t::size_type( 5), r.recent());
    CHECK_EQUAL('H', result[0]);
    CHECK_EQUAL('e', result[1]);
  }

  TEST(input) {
    std::istringstream s("Hello!");
    std::istream_iterator<char> i(s);
    using base_t = arr::buffer_base<char>;
    base_t b(20);
    arr::buffer_direction<base_t::size_type> w;
    {
      arr::buffer_transfer<base_t> t(b, w, all);
      t.write(i, 2);
    }
    CHECK_EQUAL('H', b.elements[0]);
    CHECK_EQUAL('e', b.elements[1]);
  }

  TEST(output) {
    std::istringstream s("Hello!");
    std::istream_iterator<char> i(s);
    using base_t = arr::buffer_base<char>;
    base_t b(20);
    arr::buffer_direction<base_t::size_type> w;
    arr::buffer_direction<base_t::size_type> r;
    {
      arr::buffer_transfer<base_t> t(b, w, all);
      t.write(i, 2);
    }
    std::ostringstream o("Hello!");
    std::ostream_iterator<char> j(o);
    {
      arr::buffer_transfer<base_t> t(b, r, all);
      t.read(j, 2);
    }
    CHECK_EQUAL(s.str()[0], o.str()[0]);
    CHECK_EQUAL(s.str()[1], o.str()[1]);
  }

}

SUITE(nontrivial) {

  TEST(create_destroy) {
    using base_t = arr::buffer_base<std::string>;
    base_t b(2);
    arr::buffer_direction<base_t::size_type> d;
    arr::buffer_transfer<base_t> t(b, d, all);
  }

  TEST(copy) {
    using base_t = arr::buffer_base<std::string>;
    base_t b(20);
    arr::buffer_direction<base_t::size_type> w;
    arr::buffer_direction<base_t::size_type> r;
    std::array<std::string, 2> s = { "Hello, world!", "Goodbye, world!"};
    {
      arr::buffer_transfer<base_t> t(b, w, all);
      t.write(s.begin(), s.size());
    }
    CHECK_EQUAL(base_t::size_type(2), w.recent());
    CHECK_EQUAL(s[0], b.elements[0]);
    CHECK_EQUAL(s[1], b.elements[1]);
    {
      arr::buffer_transfer<base_t> t(b, r, all);
      t.discard(s.size());
    }
    CHECK_EQUAL(base_t::size_type(2), r.recent());
  }

  TEST(move) {
    using base_t = arr::buffer_base<std::string>;
    base_t b(20);
    arr::buffer_direction<base_t::size_type> w;
    arr::buffer_direction<base_t::size_type> r;
    std::array<std::string, 2> s = { "Hello, world!", "Goodbye, world!"};
    {
      arr::buffer_transfer<base_t> t(b, w, all);
      t.write(std::make_move_iterator(s.begin()), s.size());
    }
    CHECK_EQUAL(base_t::size_type(2), w.recent());
    CHECK_EQUAL(true, s[0].empty());
    CHECK_EQUAL(true, s[1].empty());
    CHECK_EQUAL("Hello, world!", b.elements[0]);
    CHECK_EQUAL("Goodbye, world!", b.elements[1]);
    {
      arr::buffer_transfer<base_t> t(b, r, all);
      t.discard(s.size());
    }
    CHECK_EQUAL(base_t::size_type(2), r.recent());
  }

  TEST(read) {
    using base_t = arr::buffer_base<std::string>;
    base_t b(20);
    arr::buffer_direction<base_t::size_type> w;
    arr::buffer_direction<base_t::size_type> r;
    std::array<std::string, 2> i = { "Hello, world!", "Goodbye, world!"};
    std::array<std::string, 2> o = { "", "" };
    {
      arr::buffer_transfer<base_t> t(b, w, all);
      t.write(i.begin(), i.size());
    }
    {
      arr::buffer_transfer<base_t> t(b, r, all);
      t.read(o.begin(), o.size());
    }
    CHECK_EQUAL(base_t::size_type(2), w.recent());
    CHECK_EQUAL(base_t::size_type(2), r.recent());
    CHECK_EQUAL(o[0], i[0]);
    CHECK_EQUAL(o[1], i[1]);
  }

}
