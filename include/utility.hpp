//  _   _  _____ _______ _____     _____
// | \ | |/ ____|__   __|  __ \   / ____|_     _
// |  \| | (___    | |  | |  | | | |   _| |_ _| |_
// | . ` |\___ \   | |  | |  | | | |  |_   _|_   _|
// | |\  |____) |  | |  | |__| | | |____|_|   |_|
// |_| \_|_____/   |_|  |_____/   \_____|
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2021 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef NEARGYE_NSTD_UTILITY_HPP
#define NEARGYE_NSTD_UTILITY_HPP

#include <cstring>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace nstd {

namespace detail {

template <typename F, typename... Args>
using enable_if_invocable_each_t = std::enable_if_t<std::bool_constant<(std::is_invocable_v<F, Args> && ...)>::value>;

template <typename F, typename... Args>
struct is_nothrow_invocable_each : std::bool_constant<(std::is_nothrow_invocable_v<F, Args> && ...)> {};

template <typename F, typename... Args>
inline constexpr bool is_nothrow_invocable_each_v = is_nothrow_invocable_each<F, Args...>::value;

template <typename F, typename Tuple, std::size_t... I>
constexpr void apply_each_impl(F&& f, Tuple&& t, std::index_sequence<I...>) noexcept(std::bool_constant<(std::is_nothrow_invocable_v<F, std::tuple_element_t<I, Tuple>> && ...)>::value) {
  (static_cast<void>(std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t)))), ...);
}

} // namespace nstd::detail

template <typename T>
[[nodiscard]] constexpr auto move(T&& x) noexcept -> std::remove_reference_t<T>&& {
  static_assert(!std::is_const_v<std::remove_reference_t<T>>, "nstd::move requires not const type.");
  return static_cast<std::remove_reference_t<T>&&>(x);
}

template <typename T>
[[nodiscard]] constexpr auto move_if_noexcept(T& x) noexcept -> std::conditional_t<!std::is_nothrow_move_constructible_v<T> &&
                                                                                      std::is_copy_constructible_v<T>,
                                                                                  const T&, T&&> {
  static_assert(!std::is_const_v<std::remove_reference_t<T>>, "nstd::move_if_noexcept requires not const type.");
  return move(x);
}

template <typename T>
[[nodiscard]] constexpr auto move_assign_if_noexcept(T& x) noexcept -> std::conditional_t<!std::is_nothrow_move_assignable_v<T> &&
                                                                                              std::is_copy_assignable_v<T>,
                                                                                          const T&, T&&> {
  static_assert(!std::is_const_v<std::remove_reference_t<T>>, "nstd::move_assign_if_noexcept requires not const type.");
  return move(x);
}

template <typename T>
[[nodiscard]] constexpr auto forward(std::remove_reference_t<T>& t) noexcept -> T&& {
  return static_cast<T&&>(t);
}

template <typename T>
[[nodiscard]] constexpr auto forward(std::remove_reference_t<T>&& t) noexcept -> T&& {
  static_assert(!std::is_lvalue_reference_v<T>, "nstd::forward bad forward.");
  return static_cast<T&&>(t);
}

template <typename T>
[[nodiscard]] constexpr auto unforward(T&& t) noexcept(std::is_nothrow_constructible_v<std::remove_reference_t<T>, T&&>) -> std::remove_reference_t<T> {
  static_assert(std::is_constructible_v<std::remove_reference_t<T>, T&&>, "nstd::unforward requires copy constructible.");
  return forward<T>(t);
}

template <typename T>
[[nodiscard]] constexpr auto decay_copy(T&& t) noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, T&&>) -> std::decay_t<T> {
  static_assert(std::is_constructible_v<std::decay_t<T>, T&&>, "nstd::decay_copy requires copy constructible.");
  return forward<T>(t);
}

template <typename To, typename From>
[[nodiscard]] auto bit_cast(const From& src) -> std::enable_if_t<sizeof(To) == sizeof(From) &&
                                                                    std::is_trivially_copyable_v<From> &&
                                                                    std::is_trivial_v<To>,
                                                                To> {
  static_assert(sizeof(To) == sizeof(From),         "nstd::bit_cast requires sizeof(To) == sizeof(From).");
  static_assert(std::is_trivially_copyable_v<From>, "nstd::bit_cast requires From is trivially copyable.");
  static_assert(std::is_trivial_v<To>,              "nstd::bit_cast requires To is trivial.");
  To dst;
  static_cast<void>(std::memcpy(&dst, &src, sizeof(To)));
  return dst;
}

template <typename F, typename... Args>
constexpr auto invoke_each(F&& f, Args&&...args) noexcept(detail::is_nothrow_invocable_each_v<F, Args...>) -> detail::enable_if_invocable_each_t<F, Args...> {
  (static_cast<void>(std::invoke(std::forward<F>(f), std::forward<Args>(args))), ...);
}

template <typename F, typename... Args>
constexpr auto apply_each(F&& f, std::tuple<Args...>&& t) noexcept(detail::is_nothrow_invocable_each_v<F, Args...>) -> detail::enable_if_invocable_each_t<F, Args...> {
  using Tuple = decltype(t);
  detail::apply_each_impl(std::forward<F>(f),
                          std::forward<Tuple>(t),
                          std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

// https://artificial-mind.net/blog/2020/10/31/constexpr-for
template <auto Start, auto End, auto Inc, typename F>
constexpr void constexpr_for(F&& f) {
  if constexpr (Start < End) {
    //static_cast<void>(std::invoke(std::forward<F>(f), std::integral_constant<decltype(Start), Start>()));
    static_cast<void>(std::forward<F>(f)(std::integral_constant<decltype(Start), Start>{}));
    constexpr_for<Start + Inc, End, Inc>(std::forward<F>(f));
  }
}

} // namespace nstd

#endif // NEARGYE_NSTD_UTILITY_HPP
