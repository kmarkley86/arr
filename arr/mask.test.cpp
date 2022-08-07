//
// Copyright (c) 2013, 2021, 2022
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
#include <cstdint>
#include <stdexcept>
#include <type_traits>

UNIT_TEST_MAIN

using namespace arr;
using std::int8_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

SUITE(basic) {

  TEST(all_ones) {
    CHECK_EQUAL(true, std::is_same_v<uint8_t , decltype(all_ones<uint8_t >())>);
    CHECK_EQUAL(true, std::is_same_v<uint16_t, decltype(all_ones<uint16_t>())>);
    CHECK_EQUAL(true, std::is_same_v<uint32_t, decltype(all_ones<uint32_t>())>);
    CHECK_EQUAL(true, std::is_same_v<uint64_t, decltype(all_ones<uint64_t>())>);
    CHECK_EQUAL(0x00000000000000FFull, all_ones<uint8_t>());
    CHECK_EQUAL(0x000000000000FFFFull, all_ones<uint16_t>());
    CHECK_EQUAL(0x00000000FFFFFFFFull, all_ones<uint32_t>());
    CHECK_EQUAL(0xFFFFFFFFFFFFFFFFull, all_ones<uint64_t>());
  }

  TEST(low_zeros) {
    CHECK_EQUAL(0x00000000000000FFull, low_zeros<uint8_t >( 0u));
    CHECK_EQUAL(0x00000000000000FEull, low_zeros<uint8_t >( 1u));
    CHECK_EQUAL(0x00000000000000FCull, low_zeros<uint8_t >( 2u));
    CHECK_EQUAL(0x0000000000000080ull, low_zeros<uint8_t >( 7u));
    CHECK_EQUAL(0x0000000000000000ull, low_zeros<uint8_t >( 8u));

    CHECK_EQUAL(0x000000000000FFFFull, low_zeros<uint16_t>( 0u));
    CHECK_EQUAL(0x000000000000FFFEull, low_zeros<uint16_t>( 1u));
    CHECK_EQUAL(0x000000000000FFFCull, low_zeros<uint16_t>( 2u));
    CHECK_EQUAL(0x0000000000008000ull, low_zeros<uint16_t>(15u));
    CHECK_EQUAL(0x0000000000000000ull, low_zeros<uint16_t>(16u));

    CHECK_EQUAL(0x00000000FFFFFFFFull, low_zeros<uint32_t>( 0u));
    CHECK_EQUAL(0x00000000FFFFFFFEull, low_zeros<uint32_t>( 1u));
    CHECK_EQUAL(0x00000000FFFFFFFCull, low_zeros<uint32_t>( 2u));
    CHECK_EQUAL(0x0000000080000000ull, low_zeros<uint32_t>(31u));
    CHECK_EQUAL(0x0000000000000000ull, low_zeros<uint32_t>(32u));

    CHECK_EQUAL(0xFFFFFFFFFFFFFFFFull, low_zeros<uint64_t>( 0u));
    CHECK_EQUAL(0xFFFFFFFFFFFFFFFEull, low_zeros<uint64_t>( 1u));
    CHECK_EQUAL(0xFFFFFFFFFFFFFFFCull, low_zeros<uint64_t>( 2u));
    CHECK_EQUAL(0x8000000000000000ull, low_zeros<uint64_t>(63u));
    CHECK_EQUAL(0x0000000000000000ull, low_zeros<uint64_t>(64u));
  }

  TEST(low_ones) {
    CHECK_EQUAL(0x0000000000000000ull, low_ones<uint8_t >( 0u));
    CHECK_EQUAL(0x0000000000000001ull, low_ones<uint8_t >( 1u));
    CHECK_EQUAL(0x0000000000000003ull, low_ones<uint8_t >( 2u));
    CHECK_EQUAL(0x000000000000007Full, low_ones<uint8_t >( 7u));
    CHECK_EQUAL(0x00000000000000FFull, low_ones<uint8_t >( 8u));

    CHECK_EQUAL(0x0000000000000000ull, low_ones<uint16_t>( 0u));
    CHECK_EQUAL(0x0000000000000001ull, low_ones<uint16_t>( 1u));
    CHECK_EQUAL(0x0000000000000003ull, low_ones<uint16_t>( 2u));
    CHECK_EQUAL(0x0000000000007FFFull, low_ones<uint16_t>(15u));
    CHECK_EQUAL(0x000000000000FFFFull, low_ones<uint16_t>(16u));

    CHECK_EQUAL(0x0000000000000000ull, low_ones<uint32_t>( 0u));
    CHECK_EQUAL(0x0000000000000001ull, low_ones<uint32_t>( 1u));
    CHECK_EQUAL(0x0000000000000003ull, low_ones<uint32_t>( 2u));
    CHECK_EQUAL(0x000000007FFFFFFFull, low_ones<uint32_t>(31u));
    CHECK_EQUAL(0x00000000FFFFFFFFull, low_ones<uint32_t>(32u));

    CHECK_EQUAL(0x0000000000000000ull, low_ones<uint64_t>( 0u));
    CHECK_EQUAL(0x0000000000000001ull, low_ones<uint64_t>( 1u));
    CHECK_EQUAL(0x0000000000000003ull, low_ones<uint64_t>( 2u));
    CHECK_EQUAL(0x7FFFFFFFFFFFFFFFull, low_ones<uint64_t>(63u));
    CHECK_EQUAL(0xFFFFFFFFFFFFFFFFull, low_ones<uint64_t>(64u));
  }

  TEST(checked_low) {
    try {
      checked_low_zeros<uint8_t>(9u);
      CHECK_CATCH(std::invalid_argument, e);
    }
    try {
      checked_low_zeros<uint8_t>(10u);
      CHECK_CATCH(std::invalid_argument, e);
    }
    try {
      checked_low_ones<uint8_t>(9u);
      CHECK_CATCH(std::invalid_argument, e);
    }
    try {
      checked_low_ones<uint8_t>(10u);
      CHECK_CATCH(std::invalid_argument, e);
    }
  }

}

TEST(mask_width_position) {
  CHECK_EQUAL(0x00u, mask_width_position<uint8_t>(0u, 0u));
  CHECK_EQUAL(0x00u, mask_width_position<uint8_t>(0u, 1u));
  CHECK_EQUAL(0x00u, mask_width_position<uint8_t>(0u, 2u));
  CHECK_EQUAL(0x00u, mask_width_position<uint8_t>(0u, 3u));
  CHECK_EQUAL(0x00u, mask_width_position<uint8_t>(0u, 4u));
  CHECK_EQUAL(0x00u, mask_width_position<uint8_t>(0u, 5u));
  CHECK_EQUAL(0x00u, mask_width_position<uint8_t>(0u, 6u));
  CHECK_EQUAL(0x00u, mask_width_position<uint8_t>(0u, 7u));
  CHECK_EQUAL(0x01u, mask_width_position<uint8_t>(1u, 0u));
  CHECK_EQUAL(0x02u, mask_width_position<uint8_t>(1u, 1u));
  CHECK_EQUAL(0x04u, mask_width_position<uint8_t>(1u, 2u));
  CHECK_EQUAL(0x08u, mask_width_position<uint8_t>(1u, 3u));
  CHECK_EQUAL(0x10u, mask_width_position<uint8_t>(1u, 4u));
  CHECK_EQUAL(0x20u, mask_width_position<uint8_t>(1u, 5u));
  CHECK_EQUAL(0x40u, mask_width_position<uint8_t>(1u, 6u));
  CHECK_EQUAL(0x80u, mask_width_position<uint8_t>(1u, 7u));
  CHECK_EQUAL(0x03u, mask_width_position<uint8_t>(2u, 0u));
  CHECK_EQUAL(0x06u, mask_width_position<uint8_t>(2u, 1u));
  CHECK_EQUAL(0x0cu, mask_width_position<uint8_t>(2u, 2u));
  CHECK_EQUAL(0x18u, mask_width_position<uint8_t>(2u, 3u));
  CHECK_EQUAL(0x30u, mask_width_position<uint8_t>(2u, 4u));
  CHECK_EQUAL(0x60u, mask_width_position<uint8_t>(2u, 5u));
  CHECK_EQUAL(0xc0u, mask_width_position<uint8_t>(2u, 6u));
  CHECK_EQUAL(0x80u, mask_width_position<uint8_t>(2u, 7u));
  CHECK_EQUAL(0x07u, mask_width_position<uint8_t>(3u, 0u));
  CHECK_EQUAL(0x0eu, mask_width_position<uint8_t>(3u, 1u));
  CHECK_EQUAL(0x1cu, mask_width_position<uint8_t>(3u, 2u));
  CHECK_EQUAL(0x38u, mask_width_position<uint8_t>(3u, 3u));
  CHECK_EQUAL(0x70u, mask_width_position<uint8_t>(3u, 4u));
  CHECK_EQUAL(0xe0u, mask_width_position<uint8_t>(3u, 5u));
  CHECK_EQUAL(0xc0u, mask_width_position<uint8_t>(3u, 6u));
  CHECK_EQUAL(0x0fu, mask_width_position<uint8_t>(4u, 0u));
  CHECK_EQUAL(0x1eu, mask_width_position<uint8_t>(4u, 1u));
  CHECK_EQUAL(0x3cu, mask_width_position<uint8_t>(4u, 2u));
  CHECK_EQUAL(0x78u, mask_width_position<uint8_t>(4u, 3u));
  CHECK_EQUAL(0xf0u, mask_width_position<uint8_t>(4u, 4u));
  CHECK_EQUAL(0xe0u, mask_width_position<uint8_t>(4u, 5u));
  CHECK_EQUAL(0x1fu, mask_width_position<uint8_t>(5u, 0u));
  CHECK_EQUAL(0x3eu, mask_width_position<uint8_t>(5u, 1u));
  CHECK_EQUAL(0x7cu, mask_width_position<uint8_t>(5u, 2u));
  CHECK_EQUAL(0xf8u, mask_width_position<uint8_t>(5u, 3u));
  CHECK_EQUAL(0xf0u, mask_width_position<uint8_t>(5u, 4u));
  CHECK_EQUAL(0x3fu, mask_width_position<uint8_t>(6u, 0u));
  CHECK_EQUAL(0x7eu, mask_width_position<uint8_t>(6u, 1u));
  CHECK_EQUAL(0xfcu, mask_width_position<uint8_t>(6u, 2u));
  CHECK_EQUAL(0xf8u, mask_width_position<uint8_t>(5u, 3u));
  CHECK_EQUAL(0x7fu, mask_width_position<uint8_t>(7u, 0u));
  CHECK_EQUAL(0xfeu, mask_width_position<uint8_t>(7u, 1u));
  CHECK_EQUAL(0xfcu, mask_width_position<uint8_t>(7u, 2u));
  CHECK_EQUAL(0xffu, mask_width_position<uint8_t>(8u, 0u));
  CHECK_EQUAL(0xfeu, mask_width_position<uint8_t>(7u, 1u));
}

TEST(checked_mask_width_position) {
    try {
      checked_mask_width_position<uint8_t>(9u, 0u);
      CHECK_CATCH(std::invalid_argument, e);
    }
    try {
      checked_mask_width_position<uint8_t>(8u, 1u);
      CHECK_CATCH(std::invalid_argument, e);
    }
    try {
      checked_mask_width_position<uint8_t>(1u, 8u);
      CHECK_CATCH(std::invalid_argument, e);
    }
    try {
      checked_mask_width_position<uint8_t>(0u, 9u);
      CHECK_CATCH(std::invalid_argument, e);
    }
    try {
      checked_mask_width_position<uint8_t>(2u, 7u);
      CHECK_CATCH(std::invalid_argument, e);
    }
    try {
      checked_mask_width_position<uint8_t>(7u, 2u);
      CHECK_CATCH(std::invalid_argument, e);
    }
}

TEST(mask_ordered_pair) {
  CHECK_EQUAL(0x01u, mask_ordered_pair<uint8_t>(0u, 0u));
  CHECK_EQUAL(0x02u, mask_ordered_pair<uint8_t>(1u, 1u));
  CHECK_EQUAL(0x04u, mask_ordered_pair<uint8_t>(2u, 2u));
  CHECK_EQUAL(0x08u, mask_ordered_pair<uint8_t>(3u, 3u));
  CHECK_EQUAL(0x10u, mask_ordered_pair<uint8_t>(4u, 4u));
  CHECK_EQUAL(0x20u, mask_ordered_pair<uint8_t>(5u, 5u));
  CHECK_EQUAL(0x40u, mask_ordered_pair<uint8_t>(6u, 6u));
  CHECK_EQUAL(0x80u, mask_ordered_pair<uint8_t>(7u, 7u));

  CHECK_EQUAL(0x03u, mask_ordered_pair<uint8_t>(1u, 0u));
  CHECK_EQUAL(0x06u, mask_ordered_pair<uint8_t>(2u, 1u));
  CHECK_EQUAL(0x0cu, mask_ordered_pair<uint8_t>(3u, 2u));
  CHECK_EQUAL(0x18u, mask_ordered_pair<uint8_t>(4u, 3u));
  CHECK_EQUAL(0x30u, mask_ordered_pair<uint8_t>(5u, 4u));
  CHECK_EQUAL(0x60u, mask_ordered_pair<uint8_t>(6u, 5u));
  CHECK_EQUAL(0xc0u, mask_ordered_pair<uint8_t>(7u, 6u));

  CHECK_EQUAL(0x07u, mask_ordered_pair<uint8_t>(2u, 0u));
  CHECK_EQUAL(0x0eu, mask_ordered_pair<uint8_t>(3u, 1u));
  CHECK_EQUAL(0x1cu, mask_ordered_pair<uint8_t>(4u, 2u));
  CHECK_EQUAL(0x38u, mask_ordered_pair<uint8_t>(5u, 3u));
  CHECK_EQUAL(0x70u, mask_ordered_pair<uint8_t>(6u, 4u));
  CHECK_EQUAL(0xe0u, mask_ordered_pair<uint8_t>(7u, 5u));

  CHECK_EQUAL(0x0fu, mask_ordered_pair<uint8_t>(3u, 0u));
  CHECK_EQUAL(0x1eu, mask_ordered_pair<uint8_t>(4u, 1u));
  CHECK_EQUAL(0x3cu, mask_ordered_pair<uint8_t>(5u, 2u));
  CHECK_EQUAL(0x78u, mask_ordered_pair<uint8_t>(6u, 3u));
  CHECK_EQUAL(0xf0u, mask_ordered_pair<uint8_t>(7u, 4u));

  CHECK_EQUAL(0x1fu, mask_ordered_pair<uint8_t>(4u, 0u));
  CHECK_EQUAL(0x3eu, mask_ordered_pair<uint8_t>(5u, 1u));
  CHECK_EQUAL(0x7cu, mask_ordered_pair<uint8_t>(6u, 2u));
  CHECK_EQUAL(0xf8u, mask_ordered_pair<uint8_t>(7u, 3u));

  CHECK_EQUAL(0x3fu, mask_ordered_pair<uint8_t>(5u, 0u));
  CHECK_EQUAL(0x7eu, mask_ordered_pair<uint8_t>(6u, 1u));
  CHECK_EQUAL(0xfcu, mask_ordered_pair<uint8_t>(7u, 2u));

  CHECK_EQUAL(0x7fu, mask_ordered_pair<uint8_t>(6u, 0u));
  CHECK_EQUAL(0xfeu, mask_ordered_pair<uint8_t>(7u, 1u));

  CHECK_EQUAL(0xffu, mask_ordered_pair<uint8_t>(7u, 0u));
}

TEST(mask_unordered_pair) {
  CHECK_EQUAL(0x03u, mask_unordered_pair<uint8_t>(0u, 1u));
  CHECK_EQUAL(0x06u, mask_unordered_pair<uint8_t>(1u, 2u));
  CHECK_EQUAL(0x0cu, mask_unordered_pair<uint8_t>(2u, 3u));
  CHECK_EQUAL(0x18u, mask_unordered_pair<uint8_t>(3u, 4u));
  CHECK_EQUAL(0x30u, mask_unordered_pair<uint8_t>(4u, 5u));
  CHECK_EQUAL(0x60u, mask_unordered_pair<uint8_t>(5u, 6u));
  CHECK_EQUAL(0xc0u, mask_unordered_pair<uint8_t>(6u, 7u));
}

TEST(compile_time) {
  static_assert(0x03u == mask_unordered_pair<uint8_t>(0u, 1u));
  static_assert(0x06u == mask_unordered_pair<uint8_t>(1u, 2u));
  static_assert(0x0cu == mask_unordered_pair<uint8_t>(2u, 3u));
  static_assert(0x18u == mask_unordered_pair<uint8_t>(3u, 4u));
  static_assert(0x30u == mask_unordered_pair<uint8_t>(4u, 5u));
  static_assert(0x60u == mask_unordered_pair<uint8_t>(5u, 6u));
  static_assert(0xc0u == mask_unordered_pair<uint8_t>(6u, 7u));
  static_assert(0x03  == mask_unordered_pair< int8_t>(0u, 1u));
  static_assert(0x06  == mask_unordered_pair< int8_t>(1u, 2u));
  static_assert(0x0c  == mask_unordered_pair< int8_t>(2u, 3u));
  static_assert(0x18  == mask_unordered_pair< int8_t>(3u, 4u));
  static_assert(0x30  == mask_unordered_pair< int8_t>(4u, 5u));
  static_assert(0x60  == mask_unordered_pair< int8_t>(5u, 6u));
}
