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
#include "arr/fifo.hpp"
#include <algorithm>
#include <chrono>
#include <deque>
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

SUITE(waiting) {

using fifo_t = arr::fifo<std::size_t>;
using namespace std::chrono_literals;

struct timeout {
  using clock = std::chrono::system_clock;
  clock::time_point end;
  timeout(clock::duration when)
    : end(clock::now() + when)
  { }
  operator bool() const { return clock::now() < end; }
};

struct rate_limiter {
  using clock = std::chrono::system_clock;
  clock::time_point start;
  std::size_t rate; // elements per microsecond
  rate_limiter(std::size_t elements_per_us)
    : start(clock::now())
    , rate(elements_per_us)
  { }

  bool should_pause(std::size_t count) const {
    auto now = clock::now();
    auto elapsed = now - start;
    using unit = std::chrono::microseconds;
    auto usec = std::chrono::duration_cast<unit>(elapsed).count();
    auto expected = rate * usec;
    return count > expected;
  }
};

TEST(test_rate_limiter) {
  rate_limiter limiter(1u);
  CHECK_EQUAL(false, limiter.should_pause(0u));
  CHECK_EQUAL(true, limiter.should_pause(1000u));
  std::this_thread::sleep_for(1ms);
  CHECK_EQUAL(false, limiter.should_pause(1000u));
}

std::ostream& operator<<(std::ostream& o, const fifo_t::debug_info& d) {
  auto size = d.write_total - d.read_total;
  o << '[' << d.write_total << '-' << d.read_total;
  o << ':' << size;
  o << ':' << d.write_waiting << ',' << d.read_waiting << ']';
  return o;
}
struct agent_data {
  unsigned pause = 0u;
  unsigned sleep = 0u;
  std::deque<fifo_t::debug_info> details;
  void operator()(const fifo_t& fifo) {
    details.push_back(fifo.get_debug_info());
  }
};
std::ostream& operator<<(std::ostream& o, const agent_data& a) {
  o << "# pauses = " << a.pause;
  o << ", # sleeps = " << a.sleep;
  o << '\n';
  for (auto&& i : a.details) {
    o << i << '\n';
  }
  return o;
}

void producer_code(
    std::reference_wrapper<arr::test::evaluator> w_evaluator,
    std::reference_wrapper<fifo_t> w_fifo,
    std::size_t rate,
    std::size_t total)
{
  [[maybe_unused]] auto& evaluator = w_evaluator.get();
  auto& fifo      = w_fifo.get();
  rate_limiter limiter(rate);
  decltype(total) counter(0u);
  timeout stopper(10s);
  agent_data data;
  while (stopper and (counter < total)) {
    if (limiter.should_pause(counter)) {
      ++data.pause;
      std::this_thread::sleep_for(1us);
    } else {
      if (fifo.full()) {
        ++data.sleep;
        fifo.wait_for_read();
      }
      fifo.push(counter++);
    }
  }
  CHECK_EQUAL(counter, total);
  std::stringstream msg;
  msg << "producer " << data;
  std::cout << msg.str();
  // Wake consumer on timeout
  if (not stopper) {
    std::this_thread::sleep_for(1s);
    if (not fifo.full()) {
      fifo.push(counter);
    }
  }
}

void consumer_code(
    std::reference_wrapper<arr::test::evaluator> w_evaluator,
    std::reference_wrapper<fifo_t> w_fifo,
    std::size_t rate,
    std::size_t total)
{
  [[maybe_unused]] auto& evaluator = w_evaluator.get();
  auto& fifo      = w_fifo.get();
  rate_limiter limiter(rate);
  decltype(total) counter(0u);
  timeout stopper(10s);
  agent_data data;
  while (stopper and (counter < total)) {
    if (limiter.should_pause(counter)) {
      ++data.pause;
      std::this_thread::sleep_for(1us);
    } else {
      if (fifo.empty()) {
        ++data.sleep;
        fifo.wait_for_write();
      }
      auto saw = fifo.front();
      fifo.pop();
      CHECK_EQUAL(counter, saw);
      if (counter != saw) {
        std::stringstream msg;
        msg << "Failure at " << fifo.get_debug_info() << '\n';
        std::cerr << msg.str() << std::flush;
      }
      ++counter;
    }
  }
  CHECK_EQUAL(counter, total);
  std::stringstream msg;
  msg << "consumer " << data;
  std::cout << msg.str();
  // Wake producer on timeout
  if (not stopper) {
    std::this_thread::sleep_for(1s);
    if (not fifo.empty()) {
      fifo.pop();
    }
  }
}

TEST(report) {
  auto threads = std::thread::hardware_concurrency();
  std::cout << "Threads: " << threads << '\n';
}

TEST(tiny) {
  auto size = 4u;
  auto total = 1024u*1024u;
  auto p_rate = 1000000u;
  auto c_rate = 1000000u;
  fifo_t fifo(size);
  std::thread thread_p(producer_code, std::ref(evaluator), std::ref(fifo),
      p_rate, total);
  std::thread thread_c(consumer_code, std::ref(evaluator), std::ref(fifo),
      c_rate, total);
  timeout stopper(12s);
  while (stopper and (fifo.read_total() < total)) {
    std::cout << fifo.get_debug_info() << '\n';
    std::this_thread::sleep_for(250ms);
  }
  thread_p.join();
  thread_c.join();
  CHECK_EQUAL(total, fifo.read_total());
}

TEST(slow) {
  auto size = 1024u;
  auto total = 1024u*1024u;
  auto p_rate = 1u;
  auto c_rate = 1u;
  fifo_t fifo(size);
  std::thread thread_p(producer_code, std::ref(evaluator), std::ref(fifo),
      p_rate, total);
  std::thread thread_c(consumer_code, std::ref(evaluator), std::ref(fifo),
      c_rate, total);
  timeout stopper(12s);
  while (stopper and (fifo.read_total() < total)) {
    std::cout << fifo.get_debug_info() << '\n';
    std::this_thread::sleep_for(250ms);
  }
  thread_p.join();
  thread_c.join();
  CHECK_EQUAL(total, fifo.read_total());
}

TEST(pfast) {
  auto size = 1024u;
  auto total = 1024u*1024u;
  auto p_rate = 1000000u;
  auto c_rate = 1u;
  fifo_t fifo(size);
  std::thread thread_p(producer_code, std::ref(evaluator), std::ref(fifo),
      p_rate, total);
  std::thread thread_c(consumer_code, std::ref(evaluator), std::ref(fifo),
      c_rate, total);
  timeout stopper(12s);
  while (stopper and (fifo.read_total() < total)) {
    std::cout << fifo.get_debug_info() << '\n';
    std::this_thread::sleep_for(250ms);
  }
  thread_p.join();
  thread_c.join();
  CHECK_EQUAL(total, fifo.read_total());
}

TEST(cfast) {
  auto size = 1024u;
  auto total = 1024u*1024u;
  auto p_rate = 1u;
  auto c_rate = 1000000u;
  fifo_t fifo(size);
  std::thread thread_p(producer_code, std::ref(evaluator), std::ref(fifo),
      p_rate, total);
  std::thread thread_c(consumer_code, std::ref(evaluator), std::ref(fifo),
      c_rate, total);
  timeout stopper(12s);
  while (stopper and (fifo.read_total() < total)) {
    std::cout << fifo.get_debug_info() << '\n';
    std::this_thread::sleep_for(250ms);
  }
  thread_p.join();
  thread_c.join();
  CHECK_EQUAL(total, fifo.read_total());
}

TEST(fast) {
  auto size = 64u*1024u;
  auto total = 8*1024u*1024u;
  auto p_rate = 1000000u;
  auto c_rate = 1000000u;
  fifo_t fifo(size);
  std::thread thread_p(producer_code, std::ref(evaluator), std::ref(fifo),
      p_rate, total);
  std::thread thread_c(consumer_code, std::ref(evaluator), std::ref(fifo),
      c_rate, total);
  timeout stopper(12s);
  while (stopper and (fifo.read_total() < total)) {
    std::cout << fifo.get_debug_info() << '\n';
    std::this_thread::sleep_for(250ms);
  }
  thread_p.join();
  thread_c.join();
  CHECK_EQUAL(total, fifo.read_total());
}

}
