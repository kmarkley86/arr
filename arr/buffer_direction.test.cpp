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
#include "arr/buffer_direction.hpp"

UNIT_TEST_MAIN

using enum arr::wake_policy;

SUITE(base) {

  TEST(construct) {
    constexpr arr::buffer_direction<unsigned> d;
    CHECK_EQUAL(0u, d.total());
    CHECK_EQUAL(0u, d.recent());
    CHECK_EQUAL(0u, d.offset());
  }

  TEST(increase_weak) {
    arr::buffer_direction<unsigned> d;
    d.increase_weak(5u, 6u, all);
    CHECK_EQUAL(5u, d.total());
    CHECK_EQUAL(5u, d.recent());
    CHECK_EQUAL(5u, d.offset());
    d.increase_weak(3u, 6u, all);
    CHECK_EQUAL(8u, d.total());
    CHECK_EQUAL(8u, d.recent());
    CHECK_EQUAL(2u, d.offset());
    d.reset_recent();
    CHECK_EQUAL(8u, d.total());
    CHECK_EQUAL(0u, d.recent());
    CHECK_EQUAL(2u, d.offset());
    d.increase_weak(5u, 6u, all);
    CHECK_EQUAL(13u, d.total());
    CHECK_EQUAL( 5u, d.recent());
    CHECK_EQUAL( 1u, d.offset());
  }

  TEST(increase_strong) {
    arr::buffer_direction<unsigned> d;
    d.increase_strong(5u, 6u, all);
    CHECK_EQUAL(5u, d.total());
    CHECK_EQUAL(5u, d.recent());
    CHECK_EQUAL(5u, d.offset());
    d.increase_strong(3u, 6u, all);
    CHECK_EQUAL(8u, d.total());
    CHECK_EQUAL(8u, d.recent());
    CHECK_EQUAL(2u, d.offset());
    d.reset_recent();
    CHECK_EQUAL(8u, d.total());
    CHECK_EQUAL(0u, d.recent());
    CHECK_EQUAL(2u, d.offset());
    d.increase_strong(5u, 6u, all);
    CHECK_EQUAL(13u, d.total());
    CHECK_EQUAL( 5u, d.recent());
    CHECK_EQUAL( 1u, d.offset());
  }

}
