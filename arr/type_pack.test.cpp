//
// Copyright (c) 2022
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
#include "arr/type_pack.hpp"
#include <utility>
#include <type_traits>
#include <tuple>

UNIT_TEST_MAIN

template <typename... T>
using pack = arr::type_pack<T...>;

TEST(size) {
  static_assert(0 == size(pack<>{}));
  static_assert(1 == size(pack<void>{}));
  static_assert(2 == size(pack<int, char>{}));
}

TEST(contains) {
  static_assert(false == contains<int>(pack<>{}));
  static_assert(true  == contains<int>(pack<void, int, int, char>{}));
}

TEST(count) {
  static_assert(0 == count<int>(pack<>{}));
  static_assert(2 == count<int>(pack<void, int, int, char>{}));
}

TEST(comparison) {
  static_assert(true  == (pack<>{} == pack<>{}));
  static_assert(true  == (pack<void>{} == pack<void>{}));
  static_assert(false == (pack<char>{} == pack<int>{}));
  static_assert(false == (pack<>{} != pack<>{}));
  static_assert(false == (pack<void>{} != pack<void>{}));
  static_assert(true  == (pack<char>{} != pack<int>{}));
}

//
// Indexing
//

TEST(indices) {
  constexpr auto e0 = std::make_index_sequence<0>{};
  constexpr auto a0 = indices(pack<>{});
  constexpr auto e1 = std::make_index_sequence<1>{};
  constexpr auto a1 = indices(pack<void>{});
  constexpr auto e2 = std::make_index_sequence<2>{};
  constexpr auto a2 = indices(pack<int, char>{});
  static_assert(std::is_same_v<decltype(e0), decltype(a0)>);
  static_assert(std::is_same_v<decltype(e1), decltype(a1)>);
  static_assert(std::is_same_v<decltype(e2), decltype(a2)>);
}

TEST(type_at) {
  constexpr auto i = pack<void, int, int&, char>{};
  using I = decltype(i);
  static_assert(std::is_same_v<void, arr::type_pack_element<0, I>::type>);
  static_assert(std::is_same_v<int , arr::type_pack_element<1, I>::type>);
  static_assert(std::is_same_v<int&, arr::type_pack_element<2, I>::type>);
  static_assert(std::is_same_v<char, arr::type_pack_element<3, I>::type>);
  static_assert(std::is_same_v<void, decltype(type_at<0>(i))>);
  static_assert(std::is_same_v<int , decltype(type_at<1>(i))>);
  static_assert(std::is_same_v<int&, decltype(type_at<2>(i))>);
  static_assert(std::is_same_v<char, decltype(type_at<3>(i))>);
}

TEST(index_of) {
  constexpr auto i = pack<void, int, int&, char>{};
  static_assert(0 == index_of<void>(i));
  static_assert(1 == index_of<int >(i));
  static_assert(2 == index_of<int&>(i));
  static_assert(3 == index_of<char>(i));
}

//
// Modifying
//

TEST(concatenation) {
  static_assert(pack<>{}          == pack<>{}     * pack<>{}    );
  static_assert(pack<void>{}      == pack<void>{} * pack<>{}    );
  static_assert(pack<void>{}      == pack<>{}     * pack<void>{});
  static_assert(pack<int, char>{} == pack<int>{}  * pack<char>{});
}

TEST(concatenate) {
  auto e = pack<>{};
  auto i = pack<int>{};
  auto c = pack<char>{};
  static_assert(pack<int, char>{} == concatenate(e, i, e, e, c, e));
}

TEST(transform) {
  auto input = pack<int, char>{};
  auto expected = pack<const int, const char>{};
  auto actual = transform<std::add_const_t>(input);
  static_assert(expected == actual);
}

TEST(repack) {
  auto input = pack<int, char>{};
  using expected = std::tuple<int, char>;
  using actual = decltype(repack<std::tuple>(input));
  static_assert(std::is_same_v<expected, actual>);
}

//
// Filtering
//

struct my_is_const {
  template <typename T>
  constexpr bool operator()() const {
    return std::is_const_v<T>;
  }
};

TEST(filter_by_type) {
  auto input = pack<int, const char, void>{};
  auto expected = pack<const char>{};
  static_assert(expected == filter_by_type<std::is_const>(input));
  static_assert(expected == filter_by_type(my_is_const{}, input));
}

template <std::size_t I>
struct idx_ge1_type {
  constexpr bool operator()() const {
    return I >= 1;
  }
};

struct idx_ge1_inst {
  constexpr bool operator()(std::size_t i) const {
    return i >= 1;
  }
};

TEST(filter_by_index) {
  auto input = pack<int, const char, void>{};
  auto expected = pack<const char, void>{};
  static_assert(expected == filter_by_index<idx_ge1_type>(input));
  static_assert(expected == filter_by_index(idx_ge1_inst{}, input));
}

template <typename T, std::size_t I>
struct ti_type {
  constexpr bool operator()() const {
    return std::is_const_v<T> or I==0;
  }
};

struct ti_inst {
  template <typename T>
  constexpr bool operator()(std::size_t i) const {
    return std::is_const_v<T> or i==0;
  }
};

TEST(filter_by_type_and_index) {
  auto input = pack<int, const char, void>{};
  auto expected = pack<int, const char>{};
  static_assert(expected == filter_by_type_and_index<ti_type>(input));
  static_assert(expected == filter_by_type_and_index(ti_inst{}, input));
}
