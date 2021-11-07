//
// Copyright (c) 2013, 2021
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
#include "arr/fifo.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <random>
#include <vector>
#include <thread>

UNIT_TEST_MAIN

// Values transferred through the fifo
struct value_sequence {
  using value_type = std::default_random_engine::result_type;
  value_sequence(value_type seed) { engine.seed(seed); }
  value_type get() { return engine(); }
  std::default_random_engine engine;
};

// Sizes of transfers through the fifo
struct size_sequence {
  using value_type = std::default_random_engine::result_type;
  size_sequence(value_type seed, value_type min, value_type max)
    : distribution(min, max) { engine.seed(seed); }
  value_type get() { return distribution(engine); }
  std::default_random_engine engine;
  std::uniform_int_distribution<value_type> distribution;
};

template <typename value_type>
void produce(
    arr::fifo<value_type>& buf,
    value_type total_elements,
    value_type value_seed,
    value_type size_seed,
    value_type min,
    value_type max)
{
  unsigned block_count = 0;
  value_sequence value(value_seed);
  size_sequence size(size_seed, min, max);
  std::vector<value_type> values(max);
  value_type num = 0;
  while (num < total_elements) {
    auto s = size.get();
    s = std::min(s, total_elements - num);
    auto b = values.begin();
    auto e = std::next(b, s);
    for (auto f = b; f != e; ++f) { *f = value.get(); }
    while (b != e) {
      b = buf.write(b, e);
      if (buf.full()) {
        ++block_count;
      }
    }
    num += s;
  }
  std::ostringstream o;
  o << "Producer blocked " << block_count << " times\n";
  std::cout << o.str();
}

SUITE(try_random) {

  TEST(mostly_full) {
    using value_type = std::default_random_engine::result_type;
    const value_type total_elements = 10000;
    const value_type value_seed = 1234;
    arr::fifo<value_type> buf(123);

    unsigned block_count = 0;
    value_sequence value(value_seed);
    size_sequence size(56, 0, 10);
    std::vector<value_type> values(10);
    std::vector<value_type> results(10);
    value_type num = 0;

    std::thread producer(produce<value_type>, std::ref(buf),
        total_elements, value_seed, 78, 0, 100);

    while (num < total_elements) {
      auto s = size.get();
      s = std::min(s, total_elements - num);
      auto b = values.begin();
      auto e = std::next(b, s);
      for (auto f = b; f != e; ++f) { *f = value.get(); }
      auto rb = results.begin();
      auto re = std::next(rb, s);
      while (rb != re) {
        rb = buf.read(rb, re);
        if (buf.empty()) {
          ++block_count;
        }
      }
      num += s;
      CHECK_RANGE_EQUAL(values.data(), results.data(), s);
    }
    std::ostringstream o;
    o << "Consumer blocked " << block_count << " times\n";
    std::cout << o.str();
    producer.join();
  }

  TEST(mostly_empty) {
    using value_type = std::default_random_engine::result_type;
    const value_type total_elements = 10000;
    const value_type value_seed = 1234;
    arr::fifo<value_type> buf(123);

    unsigned block_count = 0;
    value_sequence value(value_seed);
    size_sequence size(56, 0, 100);
    std::vector<value_type> values(100);
    std::vector<value_type> results(100);
    value_type num = 0;

    std::thread producer(produce<value_type>, std::ref(buf),
        total_elements, value_seed, 78, 0, 10);

    while (num < total_elements) {
      auto s = size.get();
      s = std::min(s, total_elements - num);
      auto b = values.begin();
      auto e = std::next(b, s);
      for (auto f = b; f != e; ++f) { *f = value.get(); }
      auto rb = results.begin();
      auto re = std::next(rb, s);
      while (rb != re) {
        rb = buf.read(rb, re);
        if (buf.empty()) {
          ++block_count;
        }
      }
      num += s;
      CHECK_RANGE_EQUAL(values.data(), results.data(), s);
    }
    std::ostringstream o;
    o << "Consumer blocked " << block_count << " times\n";
    std::cout << o.str();
    producer.join();
  }

}
