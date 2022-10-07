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
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

TEST(all_zeros) {
  static_assert(std::is_same_v<uint8_t , decltype(all_zeros<uint8_t >())>);
  static_assert(std::is_same_v<uint16_t, decltype(all_zeros<uint16_t>())>);
  static_assert(std::is_same_v<uint32_t, decltype(all_zeros<uint32_t>())>);
  static_assert(std::is_same_v<uint64_t, decltype(all_zeros<uint64_t>())>);
  static_assert(0u == all_zeros<uint8_t>());
  static_assert(0u == all_zeros<uint16_t>());
  static_assert(0u == all_zeros<uint32_t>());
  static_assert(0u == all_zeros<uint64_t>());
  static_assert(0  == all_zeros<int8_t>());
  static_assert(0  == all_zeros<int16_t>());
  static_assert(0  == all_zeros<int32_t>());
  static_assert(0  == all_zeros<int64_t>());
  CHECK_EQUAL(true, std::is_same_v<uint8_t , decltype(all_zeros<uint8_t >())>);
  CHECK_EQUAL(true, std::is_same_v<uint16_t, decltype(all_zeros<uint16_t>())>);
  CHECK_EQUAL(true, std::is_same_v<uint32_t, decltype(all_zeros<uint32_t>())>);
  CHECK_EQUAL(true, std::is_same_v<uint64_t, decltype(all_zeros<uint64_t>())>);
  CHECK_EQUAL(0u, all_zeros<uint8_t>());
  CHECK_EQUAL(0u, all_zeros<uint16_t>());
  CHECK_EQUAL(0u, all_zeros<uint32_t>());
  CHECK_EQUAL(0u, all_zeros<uint64_t>());
  CHECK_EQUAL(0 , all_zeros<int8_t>());
  CHECK_EQUAL(0 , all_zeros<int16_t>());
  CHECK_EQUAL(0 , all_zeros<int32_t>());
  CHECK_EQUAL(0 , all_zeros<int64_t>());
}

TEST(all_ones) {
  static_assert(std::is_same_v<uint8_t , decltype(all_ones<uint8_t >())>);
  static_assert(std::is_same_v<uint16_t, decltype(all_ones<uint16_t>())>);
  static_assert(std::is_same_v<uint32_t, decltype(all_ones<uint32_t>())>);
  static_assert(std::is_same_v<uint64_t, decltype(all_ones<uint64_t>())>);
  static_assert(0x00000000000000FFull == all_ones<uint8_t>());
  static_assert(0x000000000000FFFFull == all_ones<uint16_t>());
  static_assert(0x00000000FFFFFFFFull == all_ones<uint32_t>());
  static_assert(0xFFFFFFFFFFFFFFFFull == all_ones<uint64_t>());
  static_assert(-1 == all_ones<int8_t>());
  static_assert(-1 == all_ones<int16_t>());
  static_assert(-1 == all_ones<int32_t>());
  static_assert(-1 == all_ones<int64_t>());
  CHECK_EQUAL(true, std::is_same_v<uint8_t , decltype(all_ones<uint8_t >())>);
  CHECK_EQUAL(true, std::is_same_v<uint16_t, decltype(all_ones<uint16_t>())>);
  CHECK_EQUAL(true, std::is_same_v<uint32_t, decltype(all_ones<uint32_t>())>);
  CHECK_EQUAL(true, std::is_same_v<uint64_t, decltype(all_ones<uint64_t>())>);
  CHECK_EQUAL(0x00000000000000FFull, all_ones<uint8_t>());
  CHECK_EQUAL(0x000000000000FFFFull, all_ones<uint16_t>());
  CHECK_EQUAL(0x00000000FFFFFFFFull, all_ones<uint32_t>());
  CHECK_EQUAL(0xFFFFFFFFFFFFFFFFull, all_ones<uint64_t>());
  CHECK_EQUAL(-1, all_ones<int8_t>());
  CHECK_EQUAL(-1, all_ones<int16_t>());
  CHECK_EQUAL(-1, all_ones<int32_t>());
  CHECK_EQUAL(-1, all_ones<int64_t>());
}

TEST(low_ones_unsigned) {
  static_assert(0x0000000000000000ull == low_ones<uint8_t >( 0u));
  static_assert(0x0000000000000001ull == low_ones<uint8_t >( 1u));
  static_assert(0x0000000000000003ull == low_ones<uint8_t >( 2u));
  static_assert(0x0000000000000007ull == low_ones<uint8_t >( 3u));
  static_assert(0x000000000000000full == low_ones<uint8_t >( 4u));
  static_assert(0x000000000000001full == low_ones<uint8_t >( 5u));
  static_assert(0x000000000000003full == low_ones<uint8_t >( 6u));
  static_assert(0x000000000000007full == low_ones<uint8_t >( 7u));
  static_assert(0x00000000000000ffull == low_ones<uint8_t >( 8u));
  static_assert(0x00000000000000ffull == low_ones<uint8_t >( 9u));
  static_assert(0x0000000000000000ull == low_ones<uint16_t>( 0u));
  static_assert(0x0000000000000001ull == low_ones<uint16_t>( 1u));
  static_assert(0x0000000000007FFFull == low_ones<uint16_t>(15u));
  static_assert(0x000000000000FFFFull == low_ones<uint16_t>(16u));
  static_assert(0x000000000000FFFFull == low_ones<uint16_t>(17u));
  static_assert(0x0000000000000000ull == low_ones<uint32_t>( 0u));
  static_assert(0x0000000000000001ull == low_ones<uint32_t>( 1u));
  static_assert(0x000000007FFFFFFFull == low_ones<uint32_t>(31u));
  static_assert(0x00000000FFFFFFFFull == low_ones<uint32_t>(32u));
  static_assert(0x00000000FFFFFFFFull == low_ones<uint32_t>(33u));
  static_assert(0x0000000000000000ull == low_ones<uint64_t>( 0u));
  static_assert(0x0000000000000001ull == low_ones<uint64_t>( 1u));
  static_assert(0x7FFFFFFFFFFFFFFFull == low_ones<uint64_t>(63u));
  static_assert(0xFFFFFFFFFFFFFFFFull == low_ones<uint64_t>(64u));
  static_assert(0xFFFFFFFFFFFFFFFFull == low_ones<uint64_t>(65u));
  CHECK_EQUAL(0x0000000000000000ull, low_ones<uint8_t >( 0u));
  CHECK_EQUAL(0x0000000000000001ull, low_ones<uint8_t >( 1u));
  CHECK_EQUAL(0x0000000000000003ull, low_ones<uint8_t >( 2u));
  CHECK_EQUAL(0x0000000000000007ull, low_ones<uint8_t >( 3u));
  CHECK_EQUAL(0x000000000000000full, low_ones<uint8_t >( 4u));
  CHECK_EQUAL(0x000000000000001full, low_ones<uint8_t >( 5u));
  CHECK_EQUAL(0x000000000000003full, low_ones<uint8_t >( 6u));
  CHECK_EQUAL(0x000000000000007full, low_ones<uint8_t >( 7u));
  CHECK_EQUAL(0x00000000000000ffull, low_ones<uint8_t >( 8u));
  CHECK_EQUAL(0x00000000000000ffull, low_ones<uint8_t >( 9u));
  CHECK_EQUAL(0x0000000000000000ull, low_ones<uint16_t>( 0u));
  CHECK_EQUAL(0x0000000000000001ull, low_ones<uint16_t>( 1u));
  CHECK_EQUAL(0x0000000000007FFFull, low_ones<uint16_t>(15u));
  CHECK_EQUAL(0x000000000000FFFFull, low_ones<uint16_t>(16u));
  CHECK_EQUAL(0x000000000000FFFFull, low_ones<uint16_t>(17u));
  CHECK_EQUAL(0x0000000000000000ull, low_ones<uint32_t>( 0u));
  CHECK_EQUAL(0x0000000000000001ull, low_ones<uint32_t>( 1u));
  CHECK_EQUAL(0x000000007FFFFFFFull, low_ones<uint32_t>(31u));
  CHECK_EQUAL(0x00000000FFFFFFFFull, low_ones<uint32_t>(32u));
  CHECK_EQUAL(0x00000000FFFFFFFFull, low_ones<uint32_t>(33u));
  CHECK_EQUAL(0x0000000000000000ull, low_ones<uint64_t>( 0u));
  CHECK_EQUAL(0x0000000000000001ull, low_ones<uint64_t>( 1u));
  CHECK_EQUAL(0x7FFFFFFFFFFFFFFFull, low_ones<uint64_t>(63u));
  CHECK_EQUAL(0xFFFFFFFFFFFFFFFFull, low_ones<uint64_t>(64u));
  CHECK_EQUAL(0xFFFFFFFFFFFFFFFFull, low_ones<uint64_t>(65u));
}

TEST(low_ones_signed) {
  static_assert(0x0000000000000000ll == low_ones<int8_t >( 0u));
  static_assert(0x0000000000000001ll == low_ones<int8_t >( 1u));
  static_assert(0x0000000000000003ll == low_ones<int8_t >( 2u));
  static_assert(0x0000000000000007ll == low_ones<int8_t >( 3u));
  static_assert(0x000000000000000fll == low_ones<int8_t >( 4u));
  static_assert(0x000000000000001fll == low_ones<int8_t >( 5u));
  static_assert(0x000000000000003fll == low_ones<int8_t >( 6u));
  static_assert(0x000000000000007fll == low_ones<int8_t >( 7u));
  static_assert(-1                   == low_ones<int8_t >( 8u));
  static_assert(-1                   == low_ones<int8_t >( 9u));
  static_assert(0x0000000000000000ll == low_ones<int16_t>( 0u));
  static_assert(0x0000000000000001ll == low_ones<int16_t>( 1u));
  static_assert(0x0000000000007FFFll == low_ones<int16_t>(15u));
  static_assert(-1                   == low_ones<int16_t>(16u));
  static_assert(-1                   == low_ones<int16_t>(17u));
  static_assert(0x0000000000000000ll == low_ones<int32_t>( 0u));
  static_assert(0x0000000000000001ll == low_ones<int32_t>( 1u));
  static_assert(0x000000007FFFFFFFll == low_ones<int32_t>(31u));
  static_assert(-1                   == low_ones<int32_t>(32u));
  static_assert(-1                   == low_ones<int32_t>(33u));
  static_assert(0x0000000000000000ll == low_ones<int64_t>( 0u));
  static_assert(0x0000000000000001ll == low_ones<int64_t>( 1u));
  static_assert(0x7FFFFFFFFFFFFFFFll == low_ones<int64_t>(63u));
  static_assert(-1                   == low_ones<int64_t>(64u));
  static_assert(-1                   == low_ones<int64_t>(65u));
  CHECK_EQUAL(0x0000000000000000ll, low_ones<int8_t >( 0u));
  CHECK_EQUAL(0x0000000000000001ll, low_ones<int8_t >( 1u));
  CHECK_EQUAL(0x0000000000000003ll, low_ones<int8_t >( 2u));
  CHECK_EQUAL(0x0000000000000007ll, low_ones<int8_t >( 3u));
  CHECK_EQUAL(0x000000000000000fll, low_ones<int8_t >( 4u));
  CHECK_EQUAL(0x000000000000001fll, low_ones<int8_t >( 5u));
  CHECK_EQUAL(0x000000000000003fll, low_ones<int8_t >( 6u));
  CHECK_EQUAL(0x000000000000007fll, low_ones<int8_t >( 7u));
  CHECK_EQUAL(-1                  , low_ones<int8_t >( 8u));
  CHECK_EQUAL(-1                  , low_ones<int8_t >( 9u));
  CHECK_EQUAL(0x0000000000000000ll, low_ones<int16_t>( 0u));
  CHECK_EQUAL(0x0000000000000001ll, low_ones<int16_t>( 1u));
  CHECK_EQUAL(0x0000000000007FFFll, low_ones<int16_t>(15u));
  CHECK_EQUAL(-1                  , low_ones<int16_t>(16u));
  CHECK_EQUAL(-1                  , low_ones<int16_t>(17u));
  CHECK_EQUAL(0x0000000000000000ll, low_ones<int32_t>( 0u));
  CHECK_EQUAL(0x0000000000000001ll, low_ones<int32_t>( 1u));
  CHECK_EQUAL(0x000000007FFFFFFFll, low_ones<int32_t>(31u));
  CHECK_EQUAL(-1                  , low_ones<int32_t>(32u));
  CHECK_EQUAL(-1                  , low_ones<int32_t>(33u));
  CHECK_EQUAL(0x0000000000000000ll, low_ones<int64_t>( 0u));
  CHECK_EQUAL(0x0000000000000001ll, low_ones<int64_t>( 1u));
  CHECK_EQUAL(0x7FFFFFFFFFFFFFFFll, low_ones<int64_t>(63u));
  CHECK_EQUAL(-1                  , low_ones<int64_t>(64u));
  CHECK_EQUAL(-1                  , low_ones<int64_t>(65u));
}

TEST(mask_width_position) {
  static_assert(0x0000000000000001ull == mask_width_position<uint8_t >(1u,  0u));
  static_assert(0x0000000000000002ull == mask_width_position<uint8_t >(1u,  1u));
  static_assert(0x0000000000000080ull == mask_width_position<uint8_t >(1u,  7u));
  static_assert(0x0000000000000000ull == mask_width_position<uint8_t >(1u,  8u));
  static_assert(0x0000000000000000ull == mask_width_position<uint8_t >(1u,  9u));
  static_assert(0x0000000000000001ull == mask_width_position<uint16_t>(1u,  0u));
  static_assert(0x0000000000000002ull == mask_width_position<uint16_t>(1u,  1u));
  static_assert(0x0000000000008000ull == mask_width_position<uint16_t>(1u, 15u));
  static_assert(0x0000000000000000ull == mask_width_position<uint16_t>(1u, 16u));
  static_assert(0x0000000000000000ull == mask_width_position<uint16_t>(1u, 17u));
  static_assert(0x0000000000000001ull == mask_width_position<uint32_t>(1u,  0u));
  static_assert(0x0000000000000002ull == mask_width_position<uint32_t>(1u,  1u));
  static_assert(0x0000000080000000ull == mask_width_position<uint32_t>(1u, 31u));
  static_assert(0x0000000000000000ull == mask_width_position<uint32_t>(1u, 32u));
  static_assert(0x0000000000000000ull == mask_width_position<uint32_t>(1u, 33u));
  static_assert(0x0000000000000001ull == mask_width_position<uint64_t>(1u,  0u));
  static_assert(0x0000000000000002ull == mask_width_position<uint64_t>(1u,  1u));
  static_assert(0x8000000000000000ull == mask_width_position<uint64_t>(1u, 63u));
  static_assert(0x0000000000000000ull == mask_width_position<uint64_t>(1u, 64u));
  static_assert(0x0000000000000000ull == mask_width_position<uint64_t>(1u, 65u));
  CHECK_EQUAL(0x0000000000000001ull, mask_width_position<uint8_t >(1u,  0u));
  CHECK_EQUAL(0x0000000000000002ull, mask_width_position<uint8_t >(1u,  1u));
  CHECK_EQUAL(0x0000000000000080ull, mask_width_position<uint8_t >(1u,  7u));
  CHECK_EQUAL(0x0000000000000000ull, mask_width_position<uint8_t >(1u,  8u));
  CHECK_EQUAL(0x0000000000000000ull, mask_width_position<uint8_t >(1u,  9u));
  CHECK_EQUAL(0x0000000000000001ull, mask_width_position<uint16_t>(1u,  0u));
  CHECK_EQUAL(0x0000000000000002ull, mask_width_position<uint16_t>(1u,  1u));
  CHECK_EQUAL(0x0000000000008000ull, mask_width_position<uint16_t>(1u, 15u));
  CHECK_EQUAL(0x0000000000000000ull, mask_width_position<uint16_t>(1u, 16u));
  CHECK_EQUAL(0x0000000000000000ull, mask_width_position<uint16_t>(1u, 17u));
  CHECK_EQUAL(0x0000000000000001ull, mask_width_position<uint32_t>(1u,  0u));
  CHECK_EQUAL(0x0000000000000002ull, mask_width_position<uint32_t>(1u,  1u));
  CHECK_EQUAL(0x0000000080000000ull, mask_width_position<uint32_t>(1u, 31u));
  CHECK_EQUAL(0x0000000000000000ull, mask_width_position<uint32_t>(1u, 32u));
  CHECK_EQUAL(0x0000000000000000ull, mask_width_position<uint32_t>(1u, 33u));
  CHECK_EQUAL(0x0000000000000001ull, mask_width_position<uint64_t>(1u,  0u));
  CHECK_EQUAL(0x0000000000000002ull, mask_width_position<uint64_t>(1u,  1u));
  CHECK_EQUAL(0x8000000000000000ull, mask_width_position<uint64_t>(1u, 63u));
  CHECK_EQUAL(0x0000000000000000ull, mask_width_position<uint64_t>(1u, 64u));
  CHECK_EQUAL(0x0000000000000000ull, mask_width_position<uint64_t>(1u, 65u));
}

TEST(mask_width_position_special_cases) {
  static_assert(all_zeros<uint8_t >() == mask_width_position<uint8_t >(0u, 0u));
  static_assert(all_zeros<uint16_t>() == mask_width_position<uint16_t>(0u, 0u));
  static_assert(all_zeros<uint32_t>() == mask_width_position<uint32_t>(0u, 0u));
  static_assert(all_zeros<uint64_t>() == mask_width_position<uint64_t>(0u, 0u));
  static_assert(all_zeros< int8_t >() == mask_width_position< int8_t >(0u, 0u));
  static_assert(all_zeros< int16_t>() == mask_width_position< int16_t>(0u, 0u));
  static_assert(all_zeros< int32_t>() == mask_width_position< int32_t>(0u, 0u));
  static_assert(all_zeros< int64_t>() == mask_width_position< int64_t>(0u, 0u));

  static_assert(all_ones<uint8_t >() == mask_width_position<uint8_t >( 8u, 0u));
  static_assert(all_ones<uint16_t>() == mask_width_position<uint16_t>(16u, 0u));
  static_assert(all_ones<uint32_t>() == mask_width_position<uint32_t>(32u, 0u));
  static_assert(all_ones<uint64_t>() == mask_width_position<uint64_t>(64u, 0u));
  static_assert(all_ones< int8_t >() == mask_width_position< int8_t >( 8u, 0u));
  static_assert(all_ones< int16_t>() == mask_width_position< int16_t>(16u, 0u));
  static_assert(all_ones< int32_t>() == mask_width_position< int32_t>(32u, 0u));
  static_assert(all_ones< int64_t>() == mask_width_position< int64_t>(64u, 0u));

  static_assert(all_zeros<uint8_t >() == mask_width_position<uint8_t >(0u,  8u));
  static_assert(all_zeros<uint16_t>() == mask_width_position<uint16_t>(0u, 16u));
  static_assert(all_zeros<uint32_t>() == mask_width_position<uint32_t>(0u, 32u));
  static_assert(all_zeros<uint64_t>() == mask_width_position<uint64_t>(0u, 64u));
  static_assert(all_zeros< int8_t >() == mask_width_position< int8_t >(0u,  8u));
  static_assert(all_zeros< int16_t>() == mask_width_position< int16_t>(0u, 16u));
  static_assert(all_zeros< int32_t>() == mask_width_position< int32_t>(0u, 32u));
  static_assert(all_zeros< int64_t>() == mask_width_position< int64_t>(0u, 64u));

  CHECK_EQUAL(all_zeros<uint8_t >(), mask_width_position<uint8_t >(0u, 0u));
  CHECK_EQUAL(all_zeros<uint16_t>(), mask_width_position<uint16_t>(0u, 0u));
  CHECK_EQUAL(all_zeros<uint32_t>(), mask_width_position<uint32_t>(0u, 0u));
  CHECK_EQUAL(all_zeros<uint64_t>(), mask_width_position<uint64_t>(0u, 0u));
  CHECK_EQUAL(all_zeros< int8_t >(), mask_width_position< int8_t >(0u, 0u));
  CHECK_EQUAL(all_zeros< int16_t>(), mask_width_position< int16_t>(0u, 0u));
  CHECK_EQUAL(all_zeros< int32_t>(), mask_width_position< int32_t>(0u, 0u));
  CHECK_EQUAL(all_zeros< int64_t>(), mask_width_position< int64_t>(0u, 0u));

  CHECK_EQUAL(all_ones<uint8_t >(), mask_width_position<uint8_t >( 8u, 0u));
  CHECK_EQUAL(all_ones<uint16_t>(), mask_width_position<uint16_t>(16u, 0u));
  CHECK_EQUAL(all_ones<uint32_t>(), mask_width_position<uint32_t>(32u, 0u));
  CHECK_EQUAL(all_ones<uint64_t>(), mask_width_position<uint64_t>(64u, 0u));
  CHECK_EQUAL(all_ones< int8_t >(), mask_width_position< int8_t >( 8u, 0u));
  CHECK_EQUAL(all_ones< int16_t>(), mask_width_position< int16_t>(16u, 0u));
  CHECK_EQUAL(all_ones< int32_t>(), mask_width_position< int32_t>(32u, 0u));
  CHECK_EQUAL(all_ones< int64_t>(), mask_width_position< int64_t>(64u, 0u));

  CHECK_EQUAL(all_zeros<uint8_t >(), mask_width_position<uint8_t >(0u,  8u));
  CHECK_EQUAL(all_zeros<uint16_t>(), mask_width_position<uint16_t>(0u, 16u));
  CHECK_EQUAL(all_zeros<uint32_t>(), mask_width_position<uint32_t>(0u, 32u));
  CHECK_EQUAL(all_zeros<uint64_t>(), mask_width_position<uint64_t>(0u, 64u));
  CHECK_EQUAL(all_zeros< int8_t >(), mask_width_position< int8_t >(0u,  8u));
  CHECK_EQUAL(all_zeros< int16_t>(), mask_width_position< int16_t>(0u, 16u));
  CHECK_EQUAL(all_zeros< int32_t>(), mask_width_position< int32_t>(0u, 32u));
  CHECK_EQUAL(all_zeros< int64_t>(), mask_width_position< int64_t>(0u, 64u));
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
  static_assert(0x01u == mask_ordered_pair<uint8_t>(0u, 0u));
  static_assert(0x02u == mask_ordered_pair<uint8_t>(1u, 1u));
  static_assert(0x04u == mask_ordered_pair<uint8_t>(2u, 2u));
  static_assert(0x08u == mask_ordered_pair<uint8_t>(3u, 3u));
  static_assert(0x10u == mask_ordered_pair<uint8_t>(4u, 4u));
  static_assert(0x20u == mask_ordered_pair<uint8_t>(5u, 5u));
  static_assert(0x40u == mask_ordered_pair<uint8_t>(6u, 6u));
  static_assert(0x80u == mask_ordered_pair<uint8_t>(7u, 7u));

  static_assert(0x03u == mask_ordered_pair<uint8_t>(1u, 0u));
  static_assert(0x06u == mask_ordered_pair<uint8_t>(2u, 1u));
  static_assert(0x0cu == mask_ordered_pair<uint8_t>(3u, 2u));
  static_assert(0x18u == mask_ordered_pair<uint8_t>(4u, 3u));
  static_assert(0x30u == mask_ordered_pair<uint8_t>(5u, 4u));
  static_assert(0x60u == mask_ordered_pair<uint8_t>(6u, 5u));
  static_assert(0xc0u == mask_ordered_pair<uint8_t>(7u, 6u));

  static_assert(0x07u == mask_ordered_pair<uint8_t>(2u, 0u));
  static_assert(0x0eu == mask_ordered_pair<uint8_t>(3u, 1u));
  static_assert(0x1cu == mask_ordered_pair<uint8_t>(4u, 2u));
  static_assert(0x38u == mask_ordered_pair<uint8_t>(5u, 3u));
  static_assert(0x70u == mask_ordered_pair<uint8_t>(6u, 4u));
  static_assert(0xe0u == mask_ordered_pair<uint8_t>(7u, 5u));

  static_assert(0x0fu == mask_ordered_pair<uint8_t>(3u, 0u));
  static_assert(0x1eu == mask_ordered_pair<uint8_t>(4u, 1u));
  static_assert(0x3cu == mask_ordered_pair<uint8_t>(5u, 2u));
  static_assert(0x78u == mask_ordered_pair<uint8_t>(6u, 3u));
  static_assert(0xf0u == mask_ordered_pair<uint8_t>(7u, 4u));

  static_assert(0x1fu == mask_ordered_pair<uint8_t>(4u, 0u));
  static_assert(0x3eu == mask_ordered_pair<uint8_t>(5u, 1u));
  static_assert(0x7cu == mask_ordered_pair<uint8_t>(6u, 2u));
  static_assert(0xf8u == mask_ordered_pair<uint8_t>(7u, 3u));

  static_assert(0x3fu == mask_ordered_pair<uint8_t>(5u, 0u));
  static_assert(0x7eu == mask_ordered_pair<uint8_t>(6u, 1u));
  static_assert(0xfcu == mask_ordered_pair<uint8_t>(7u, 2u));

  static_assert(0x7fu == mask_ordered_pair<uint8_t>(6u, 0u));
  static_assert(0xfeu == mask_ordered_pair<uint8_t>(7u, 1u));

  static_assert(0xffu == mask_ordered_pair<uint8_t>(7u, 0u));

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
  static_assert(0x03u == mask_unordered_pair<uint8_t>(0u, 1u));
  static_assert(0x06u == mask_unordered_pair<uint8_t>(1u, 2u));
  static_assert(0x0cu == mask_unordered_pair<uint8_t>(2u, 3u));
  static_assert(0x18u == mask_unordered_pair<uint8_t>(3u, 4u));
  static_assert(0x30u == mask_unordered_pair<uint8_t>(4u, 5u));
  static_assert(0x60u == mask_unordered_pair<uint8_t>(5u, 6u));
  static_assert(0xc0u == mask_unordered_pair<uint8_t>(6u, 7u));
  CHECK_EQUAL(0x03u, mask_unordered_pair<uint8_t>(0u, 1u));
  CHECK_EQUAL(0x06u, mask_unordered_pair<uint8_t>(1u, 2u));
  CHECK_EQUAL(0x0cu, mask_unordered_pair<uint8_t>(2u, 3u));
  CHECK_EQUAL(0x18u, mask_unordered_pair<uint8_t>(3u, 4u));
  CHECK_EQUAL(0x30u, mask_unordered_pair<uint8_t>(4u, 5u));
  CHECK_EQUAL(0x60u, mask_unordered_pair<uint8_t>(5u, 6u));
  CHECK_EQUAL(0xc0u, mask_unordered_pair<uint8_t>(6u, 7u));
  static_assert(int8_t(0x03) == mask_unordered_pair< int8_t>(0u, 1u));
  static_assert(int8_t(0x06) == mask_unordered_pair< int8_t>(1u, 2u));
  static_assert(int8_t(0x0c) == mask_unordered_pair< int8_t>(2u, 3u));
  static_assert(int8_t(0x18) == mask_unordered_pair< int8_t>(3u, 4u));
  static_assert(int8_t(0x30) == mask_unordered_pair< int8_t>(4u, 5u));
  static_assert(int8_t(0x60) == mask_unordered_pair< int8_t>(5u, 6u));
  static_assert(int8_t(0xc0) == mask_unordered_pair< int8_t>(6u, 7u));
  CHECK_EQUAL(int8_t(0x03), mask_unordered_pair< int8_t>(0u, 1u));
  CHECK_EQUAL(int8_t(0x06), mask_unordered_pair< int8_t>(1u, 2u));
  CHECK_EQUAL(int8_t(0x0c), mask_unordered_pair< int8_t>(2u, 3u));
  CHECK_EQUAL(int8_t(0x18), mask_unordered_pair< int8_t>(3u, 4u));
  CHECK_EQUAL(int8_t(0x30), mask_unordered_pair< int8_t>(4u, 5u));
  CHECK_EQUAL(int8_t(0x60), mask_unordered_pair< int8_t>(5u, 6u));
  CHECK_EQUAL(int8_t(0xc0), mask_unordered_pair< int8_t>(6u, 7u));
}
