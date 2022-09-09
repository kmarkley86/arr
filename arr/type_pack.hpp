#ifndef ARR_TYPE_PACK_HPP
#define ARR_TYPE_PACK_HPP
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

#include <cstddef>
#include <type_traits>
#include <utility>

namespace arr {

//
// Machinery for indexing type packs
//
namespace detail {

template <std::size_t Position, typename Type>
struct type_pack_index_element {
  using type = Type;
};

template <typename Indices, typename Pack>
struct type_pack_index;

//
// Specialization gives us access to the packed types, so the index can be
// built by inheriting from all of its elements
//
template <
  template <typename...> typename Pack,
  std::size_t... Indices,
  typename... Types>
struct type_pack_index<std::index_sequence<Indices...>, Pack<Types...>>
  : type_pack_index_element<Indices, Types>...
{ };

//
// Identify the specific element from an index by passing the entire index,
// forcing the unique compatible base class to be chosen as the parameter
//
template <std::size_t Position, typename Index>
type_pack_index_element<Position, Index>
type_at_index(type_pack_index_element<Position, Index>);

//
// Alias for the correct type_pack_index_element
//
template <
  std::size_t Index,
  template <typename...> typename Pack,
  typename... Types>
using element_at = decltype(type_at_index<Index>(
      type_pack_index<
        std::index_sequence_for<Types...>, Pack<Types...>>{}));

}


template <typename... Types> struct type_pack {};

template <typename... Types>
constexpr auto size(type_pack<Types...>) {
  return sizeof...(Types);
}

template <typename T, typename... Types>
constexpr auto contains(type_pack<Types...>) {
  return (... or std::is_same_v<T, Types>);
}

template <typename T, typename... Types>
constexpr auto count(type_pack<Types...>) {
  return (std::size_t(0) + ... + std::is_same_v<T, Types>);
}

template <typename... Ts, typename... Us>
constexpr bool operator==(type_pack<Ts...> a, type_pack<Us...> b) {
  return std::is_same_v<decltype(a), decltype(b)>;
}

template <typename... Ts, typename... Us>
constexpr bool operator!=(type_pack<Ts...> a, type_pack<Us...> b) {
  return not (a == b);
}

//
// Indexing
//

template <typename... Types>
constexpr auto indices(type_pack<Types...>) {
  return std::index_sequence_for<Types...>{};
}

template <std::size_t Index, typename Pack> struct type_pack_element;

template <std::size_t Index, typename... Types>
struct type_pack_element<Index, type_pack<Types...>>
{
  static_assert(Index < sizeof...(Types), "type_pack index out of range");
  using type = typename detail::element_at<Index, type_pack, Types...>::type;
};

template <std::size_t Index, typename... Types>
struct type_pack_element<Index, const type_pack<Types...>>
{
  using type = typename type_pack_element<Index, type_pack<Types...>>::type;
};

template <std::size_t Index, typename... Types>
constexpr auto type_at(type_pack<Types...>)
  -> typename type_pack_element<Index, type_pack<Types...>>::type
{
  return {};
}
// usage is usually: decltype(type_at<N>(type_pack_instance))

template <typename T, typename... Types, size_t... Indices>
constexpr std::size_t index_of(
    type_pack<Types...>, std::index_sequence<Indices...>) {
  return (std::size_t(0) + ... + (Indices * std::is_same_v<T, Types>));
}

template <typename T, typename... Types>
constexpr std::size_t index_of(type_pack<Types...> p)
{
  constexpr auto instances = count<T>(p);
  static_assert(instances != 0, "type_pack does not contain type");
  static_assert(instances <  2, "type_pack contains type more than once");
  return index_of<T>(p, indices(p));
}

//
// Modifying
//

template <typename... Ts, typename... Us>
constexpr type_pack<Ts..., Us...>
operator||(type_pack<Ts...>, type_pack<Us...>) { // concatenation
  return {};
}

template <typename... Packs>
constexpr auto concatenate(Packs... type_packs) {
  return (... || type_packs);
}

template <template <typename> typename T, typename... Ts>
constexpr type_pack<T<Ts>...> transform(type_pack<Ts...>) {
  return {};
}

template <template <typename...> typename T, typename... Ts>
constexpr T<Ts...> repack(type_pack<Ts...>) {
  return {};
}
// usage is usually: decltype(repack<target>(type_pack_instance))

//
// Filtering
//

template <bool Pred, typename T>
using conditional_pack = std::conditional_t<Pred, type_pack<T>, type_pack<>>;

template <template <typename> typename Pred, typename... Types>
constexpr auto filter_by_type(type_pack<Types...>) {
  return concatenate(conditional_pack<Pred<Types>{}(), Types>{}...);
}

template <typename Pred, typename... Types>
constexpr auto filter_by_type(Pred pred, type_pack<Types...>) {
  return concatenate(conditional_pack<
      pred.template operator()<Types>(), Types>{}...);
}

template <template <std::size_t> typename Pred,
         typename... Types, size_t... Indices>
constexpr auto filter_by_index(
    type_pack<Types...>, std::index_sequence<Indices...>) {
  return concatenate(conditional_pack<Pred<Indices>{}(), Types>{}...);
}

template <template <std::size_t> typename Pred, typename... Types>
constexpr auto filter_by_index(type_pack<Types...> p) {
  return filter_by_index<Pred>(p, indices(p));
}

template <typename Pred, typename... Types, size_t... Indices>
constexpr auto filter_by_index(
    Pred pred, type_pack<Types...>, std::index_sequence<Indices...>) {
  return concatenate(conditional_pack<pred.operator()(Indices), Types>{}...);
}

template <typename Pred, typename... Types>
constexpr auto filter_by_index(Pred pred, type_pack<Types...> p) {
  return filter_by_index(pred, p, indices(p));
}

template <template <typename, std::size_t> typename Pred,
         typename... Types, size_t... Indices>
constexpr auto filter_by_type_and_index(
    type_pack<Types...>, std::index_sequence<Indices...>) {
  return concatenate(conditional_pack<Pred<Types, Indices>{}(), Types>{}...);
}

template <template <typename, std::size_t> typename Pred, typename... Types>
constexpr auto filter_by_type_and_index(type_pack<Types...> p) {
  return filter_by_type_and_index<Pred>(p, indices(p));
}

template <typename Pred, typename... Types, size_t... Indices>
constexpr auto filter_by_type_and_index(
    Pred pred, type_pack<Types...>, std::index_sequence<Indices...>) {
  return concatenate(conditional_pack<
      pred.template operator()<Types>(Indices), Types>{}...);
}

template <typename Pred, typename... Types>
constexpr auto filter_by_type_and_index(Pred pred, type_pack<Types...> p) {
  return filter_by_type_and_index(pred, p, indices(p));
}

}
#endif
