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

#include "arr/mask.hpp"
#include "arrtest/arrtest.hpp"
#include <ostream>
#include <algorithm>

UNIT_TEST_MAIN

SUITE(basic) {

  TEST(all_ones) {
    using type = unsigned int;
    constexpr auto actual = arr::all_ones<type>();
    CHECK_EQUAL(std::numeric_limits<type>::max(), actual);
  }

  TEST(partial_low_mask) {
    using type = unsigned int;
    type expected = 0;
    for (unsigned i=0; i<std::numeric_limits<type>::digits; ++i) {
      auto actual = arr::partial_low_mask<type>(i);
      CHECK_EQUAL(expected, actual);
      expected = expected + expected + 1;
    }
    constexpr auto actual_0 = arr::partial_low_mask<type>(0);
    constexpr auto actual_1 = arr::partial_low_mask<type>(1);
    constexpr auto actual_2 = arr::partial_low_mask<type>(2);
    CHECK_EQUAL(type(0x0), actual_0);
    CHECK_EQUAL(type(0x1), actual_1);
    CHECK_EQUAL(type(0x3), actual_2);
  }

  TEST(low_mask) {
    using type = unsigned int;
    constexpr auto digits = std::numeric_limits<type>::digits;
    constexpr auto is_signed = std::numeric_limits<type>::is_signed;
    constexpr auto num_full    = digits + is_signed;
    constexpr auto num_partial = num_full - 1;
    constexpr auto num_over    = num_full + 1;
    constexpr auto actual_f = arr::low_mask<type>(num_full);
    constexpr auto actual_p = arr::low_mask<type>(num_partial);
    constexpr auto actual_o = arr::low_mask<type>(num_over);
    CHECK_EQUAL(arr::all_ones<type>(), actual_f);
    CHECK_EQUAL(arr::partial_low_mask<type>(num_partial), actual_p);
    CHECK_EQUAL(arr::all_ones<type>(), actual_o);
  }

  TEST(low_mask_checked) {
    using type = unsigned int;
    constexpr auto digits = std::numeric_limits<type>::digits;
    constexpr auto is_signed = std::numeric_limits<type>::is_signed;
    constexpr auto num_full    = digits + is_signed;
    constexpr auto num_partial = num_full - 1;
    constexpr auto num_over    = num_full + 1;
    constexpr auto actual_f = arr::low_mask_checked<type>(num_full);
    constexpr auto actual_p = arr::low_mask_checked<type>(num_partial);
    try {
      arr::low_mask_checked<type>(num_over);
      CHECK_CATCH(std::invalid_argument, e);
      static_cast<void>(e);
    }
    CHECK_EQUAL(arr::all_ones<type>(), actual_f);
    CHECK_EQUAL(arr::partial_low_mask<type>(num_partial), actual_p);
  }

}

template <typename T> T inline tedious_mask(unsigned x, unsigned y) {
  auto bounds = std::minmax(x, y);
  T r(0);
  for (unsigned i=bounds.first; i<=bounds.second; ++i) {
    r |= T(1) << i;
  }
  return r;
}

SUITE(masks) {

  TEST(mask) {
    using type = unsigned int;
    constexpr auto digits = std::numeric_limits<type>::digits;
    constexpr auto is_signed = std::numeric_limits<type>::is_signed;
    constexpr auto width = digits + is_signed;
    for (unsigned l=0; l<width; ++l) {
      for (unsigned h=l; h<width; ++h) {
        auto expected = tedious_mask<type>(h, l);
        auto actual   = arr::mask<type>(h, l);
        CHECK_EQUAL(expected, actual);
      }
    }
    constexpr auto actual_21 = arr::mask<type>(2, 1);
    CHECK_EQUAL(type(0x6), actual_21);
  }

  TEST(mask_checked) {
    using type = unsigned int;
    constexpr auto digits = std::numeric_limits<type>::digits;
    constexpr auto is_signed = std::numeric_limits<type>::is_signed;
    constexpr auto width = digits + is_signed;
    for (unsigned l=0; l<width; ++l) {
      for (unsigned h=l; h<width; ++h) {
        auto expected = tedious_mask<type>(h, l);
        auto actual   = arr::mask_checked<type>(h, l);
        CHECK_EQUAL(expected, actual);
      }
    }
    constexpr auto actual_21 = arr::mask_checked<type>(2, 1);
    CHECK_EQUAL(type(0x6), actual_21);
    try {
      arr::mask_checked<type>(width, width-1);
      CHECK_CATCH(std::invalid_argument, e);
      static_cast<void>(e);
    }
  }

  TEST(mask_unordered) {
    using type = unsigned int;
    constexpr auto digits = std::numeric_limits<type>::digits;
    constexpr auto is_signed = std::numeric_limits<type>::is_signed;
    constexpr auto width = digits + is_signed;
    for (unsigned x=0; x<width; ++x) {
      for (unsigned y=0; y<width; ++y) {
        auto expected = tedious_mask<type>(x, y);
        auto actual   = arr::mask_unordered<type>(x, y);
        CHECK_EQUAL(expected, actual);
      }
    }
    constexpr auto actual_21 = arr::mask_unordered<type>(2, 1);
    constexpr auto actual_12 = arr::mask_unordered<type>(1, 2);
    CHECK_EQUAL(type(0x6), actual_21);
    CHECK_EQUAL(type(0x6), actual_12);
  }

  TEST(mask_unordered_checked) {
    using type = unsigned int;
    constexpr auto digits = std::numeric_limits<type>::digits;
    constexpr auto is_signed = std::numeric_limits<type>::is_signed;
    constexpr auto width = digits + is_signed;
    for (unsigned x=0; x<width; ++x) {
      for (unsigned y=0; y<width; ++y) {
        auto expected = tedious_mask<type>(x, y);
        auto actual   = arr::mask_unordered_checked<type>(x, y);
        CHECK_EQUAL(expected, actual);
      }
    }
    constexpr auto actual_21 = arr::mask_unordered_checked<type>(2, 1);
    constexpr auto actual_12 = arr::mask_unordered_checked<type>(1, 2);
    CHECK_EQUAL(type(0x6), actual_21);
    CHECK_EQUAL(type(0x6), actual_12);
    try {
      arr::mask_unordered_checked<type>(width, width-1);
      CHECK_CATCH(std::invalid_argument, e);
      static_cast<void>(e);
    }
  }

}
