//  _   _  _____ _______ _____     _____
// | \ | |/ ____|__   __|  __ \   / ____|_     _
// |  \| | (___    | |  | |  | | | |   _| |_ _| |_
// | . ` |\___ \   | |  | |  | | | |  |_   _|_   _|
// | |\  |____) |  | |  | |__| | | |____|_|   |_|
// |_| \_|_____/   |_|  |_____/   \_____|
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.
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

#include <array>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace nstd {

namespace detail {

template <typename F, typename... Args>
using enable_if_invocable_each_t = std::enable_if_t<(std::is_invocable_v<F&, Args> && ...)>;

template <typename F, typename... Args>
inline constexpr bool is_nothrow_invocable_each_v = (std::is_nothrow_invocable_v<F&, Args> && ...);

template <typename F, typename Tuple, std::size_t... I>
auto test_applicable_each(std::index_sequence<I...>) -> std::bool_constant<(std::is_invocable_v<F&, decltype(std::get<I>(std::declval<Tuple>()))> && ...)>;

template <typename, typename>
auto test_applicable_each(...) -> std::false_type;

template <typename F, typename Tuple, std::size_t... I>
auto test_nothrow_applicable_each(std::index_sequence<I...>) -> std::bool_constant<(std::is_nothrow_invocable_v<F&, decltype(std::get<I>(std::declval<Tuple>()))> && ...)>;

template <typename, typename>
auto test_nothrow_applicable_each(...) -> std::false_type;

template <typename F, typename Tuple, typename = void>
struct apply_each_traits {
  static constexpr bool is_applicable = false;
  static constexpr bool is_nothrow = false;
};

template <typename F, typename Tuple>
struct apply_each_traits<F, Tuple, std::void_t<decltype(std::tuple_size<std::remove_reference_t<Tuple>>::value)>> {
  using indices = std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>;

  static constexpr bool is_applicable = decltype(test_applicable_each<F, Tuple>(indices{}))::value;
  static constexpr bool is_nothrow = decltype(test_nothrow_applicable_each<F, Tuple>(indices{}))::value;
};

template <typename F, typename Tuple, std::size_t... I>
constexpr void apply_each_impl(F&& f, Tuple&& t, std::index_sequence<I...>) noexcept(apply_each_traits<F, Tuple&&>::is_nothrow) {
  (static_cast<void>(std::invoke(f, std::get<I>(std::forward<Tuple>(t)))), ...);
}

template <typename From, typename To>
using copy_const_t = std::conditional_t<std::is_const_v<From>, std::add_const_t<To>, To>;

template <typename T, typename U>
using forward_like_t = std::conditional_t<std::is_lvalue_reference_v<T&&>,
                                         copy_const_t<std::remove_reference_t<T>, std::remove_reference_t<U>>&,
                                         copy_const_t<std::remove_reference_t<T>, std::remove_reference_t<U>>&&>;

template <typename T>
using enable_if_mutable_t = std::enable_if_t<!std::is_const_v<std::remove_reference_t<T>>, int>;

template <typename T>
using enable_if_referenceable_t = std::enable_if_t<std::is_reference_v<std::add_lvalue_reference_t<T>>, int>;

template <typename T>
inline constexpr bool is_standard_integer_v = std::is_integral_v<T> &&
    !std::is_same_v<std::remove_cv_t<T>, bool> &&
    !std::is_same_v<std::remove_cv_t<T>, char> &&
    !std::is_same_v<std::remove_cv_t<T>, wchar_t> &&
    !std::is_same_v<std::remove_cv_t<T>, char16_t> &&
    !std::is_same_v<std::remove_cv_t<T>, char32_t>
#if defined(__cpp_char8_t)
    && !std::is_same_v<std::remove_cv_t<T>, char8_t>
#endif
    ;

template <typename T>
using require_integral_t = std::enable_if_t<is_standard_integer_v<T>, int>;

template <typename T, typename U>
using require_integral_pair_t = std::enable_if_t<is_standard_integer_v<T> && is_standard_integer_v<U>, int>;

template <auto Start, auto End, auto Inc,
          bool = std::is_integral_v<decltype(Start)> &&
                 !std::is_same_v<decltype(Start), bool> &&
                 std::is_same_v<decltype(Start), decltype(End)> &&
                 std::is_same_v<decltype(Start), decltype(Inc)>>
struct is_valid_constexpr_for_range : std::false_type {};

template <auto Start, auto End, auto Inc>
struct is_valid_constexpr_for_range<Start, End, Inc, true> : std::bool_constant<(Inc > 0)> {};

template <typename T>
constexpr bool can_increment_constexpr_for(T value, T increment) noexcept {
  return value <= (std::numeric_limits<T>::max)() - increment;
}

template <typename F, typename I, typename = void>
struct is_constexpr_for_callable : std::false_type {};

template <typename F, typename I>
struct is_constexpr_for_callable<F, I, std::void_t<decltype(std::declval<F&>()(std::declval<I>()))>> : std::true_type {};

template <auto Start, auto End, auto Inc, typename F, bool Done = !(Start < End), bool CanIncrement = can_increment_constexpr_for(Start, Inc)>
struct is_constexpr_for_invocable;

template <auto Start, auto End, auto Inc, typename F, bool CanIncrement>
struct is_constexpr_for_invocable<Start, End, Inc, F, true, CanIncrement> : std::true_type {};

template <auto Start, auto End, auto Inc, typename F>
struct is_constexpr_for_invocable<Start, End, Inc, F, false, false> : is_constexpr_for_callable<F, std::integral_constant<decltype(Start), Start>> {};

template <auto Start, auto End, auto Inc, typename F>
struct is_constexpr_for_invocable<Start, End, Inc, F, false, true>
    : std::conjunction<is_constexpr_for_callable<F, std::integral_constant<decltype(Start), Start>>,
                       is_constexpr_for_invocable<static_cast<decltype(Start)>(Start + Inc), End, Inc, F>> {};

template <auto Start, auto End, auto Inc, typename F, bool = is_valid_constexpr_for_range<Start, End, Inc>::value>
struct constexpr_for_traits : std::false_type {};

template <auto Start, auto End, auto Inc, typename F>
struct constexpr_for_traits<Start, End, Inc, F, true> : is_constexpr_for_invocable<Start, End, Inc, F> {};

template <typename T>
using bit_cast_return_source_t = std::conditional_t<std::is_trivially_copy_constructible_v<T>, const T&, T&&>;

template <typename To, typename From>
using enable_if_bit_cast_t = std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To> &&
                                             !std::is_volatile_v<From> && !std::is_const_v<To> && !std::is_volatile_v<To> &&
                                             std::is_default_constructible_v<To> && std::is_trivially_constructible_v<To, bit_cast_return_source_t<To>>, To>;

} // namespace nstd::detail

template <typename T, detail::enable_if_mutable_t<T> = 0>
[[nodiscard]] constexpr auto move(T&& x) noexcept -> std::remove_reference_t<T>&& {
  return static_cast<std::remove_reference_t<T>&&>(x);
}

template <typename T, detail::enable_if_mutable_t<T> = 0>
[[nodiscard]] constexpr auto move_if_noexcept(T& x) noexcept -> std::conditional_t<!std::is_nothrow_move_constructible_v<T> && std::is_copy_constructible_v<T>, const T&, T&&> {
  return ::nstd::move(x);
}

template <typename T, detail::enable_if_mutable_t<T> = 0>
[[nodiscard]] constexpr auto move_assign_if_noexcept(T& x) noexcept -> std::conditional_t<!std::is_nothrow_move_assignable_v<T> && std::is_copy_assignable_v<T>, const T&, T&&> {
  return ::nstd::move(x);
}

template <typename T>
[[nodiscard]] constexpr auto forward(std::remove_reference_t<T>& t) noexcept -> T&& {
  return static_cast<T&&>(t);
}

template <typename T, std::enable_if_t<!std::is_lvalue_reference_v<T>, int> = 0>
[[nodiscard]] constexpr auto forward(std::remove_reference_t<T>&& t) noexcept -> T&& {
  return static_cast<T&&>(t);
}

template <typename T, std::enable_if_t<std::is_constructible_v<std::remove_reference_t<T>, T&&>, int> = 0>
[[nodiscard]] constexpr auto unforward(T&& t) noexcept(std::is_nothrow_constructible_v<std::remove_reference_t<T>, T&&>) -> std::remove_reference_t<T> {
  return std::remove_reference_t<T>(::nstd::forward<T>(t));
}

template <typename T, std::enable_if_t<std::is_constructible_v<std::decay_t<T>, T&&>, int> = 0>
[[nodiscard]] constexpr auto decay_copy(T&& t) noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, T&&>) -> std::decay_t<T> {
  return std::decay_t<T>(::nstd::forward<T>(t));
}

template <typename To, typename From>
[[nodiscard]] auto bit_cast(const From& src) noexcept(std::is_nothrow_default_constructible_v<To> && std::is_nothrow_constructible_v<To, detail::bit_cast_return_source_t<To>>) -> detail::enable_if_bit_cast_t<To, From> {
  To dst;
  static_cast<void>(std::memcpy(static_cast<void*>(std::addressof(dst)), std::addressof(src), sizeof(To)));
  return To(static_cast<detail::bit_cast_return_source_t<To>>(dst));
}

template <typename F, typename... Args>
constexpr auto invoke_each(F&& f, Args&&... args) noexcept(detail::is_nothrow_invocable_each_v<F, Args&&...>) -> detail::enable_if_invocable_each_t<F, Args&&...> {
  (static_cast<void>(std::invoke(f, std::forward<Args>(args))), ...);
}

template <typename F, typename Tuple>
constexpr auto apply_each(F&& f, Tuple&& t) noexcept(detail::apply_each_traits<F, Tuple&&>::is_nothrow) -> std::enable_if_t<detail::apply_each_traits<F, Tuple&&>::is_applicable> {
  detail::apply_each_impl(std::forward<F>(f), std::forward<Tuple>(t), typename detail::apply_each_traits<F, Tuple&&>::indices{});
}

template <typename T, typename U, detail::enable_if_referenceable_t<T> = 0>
[[nodiscard]] constexpr auto forward_like(U&& u) noexcept -> detail::forward_like_t<T, U> {
  return static_cast<detail::forward_like_t<T, U>>(u);
}

template <typename E>
[[nodiscard]] constexpr auto to_underlying(E value) noexcept -> std::enable_if_t<std::is_enum_v<E>, std::underlying_type_t<E>> {
  return static_cast<std::underlying_type_t<E>>(value);
}

template <typename T, typename U, detail::require_integral_pair_t<T, U> = 0>
[[nodiscard]] constexpr bool cmp_equal(T lhs, U rhs) noexcept {
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
    return lhs == rhs;
  } else if constexpr (std::is_signed_v<T>) {
    return lhs >= 0 && static_cast<std::make_unsigned_t<T>>(lhs) == rhs;
  } else {
    return rhs >= 0 && lhs == static_cast<std::make_unsigned_t<U>>(rhs);
  }
}

template <typename T, typename U, detail::require_integral_pair_t<T, U> = 0>
[[nodiscard]] constexpr bool cmp_not_equal(T lhs, U rhs) noexcept {
  return !cmp_equal(lhs, rhs);
}

template <typename T, typename U, detail::require_integral_pair_t<T, U> = 0>
[[nodiscard]] constexpr bool cmp_less(T lhs, U rhs) noexcept {
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
    return lhs < rhs;
  } else if constexpr (std::is_signed_v<T>) {
    return lhs < 0 || static_cast<std::make_unsigned_t<T>>(lhs) < rhs;
  } else {
    return rhs >= 0 && lhs < static_cast<std::make_unsigned_t<U>>(rhs);
  }
}

template <typename T, typename U, detail::require_integral_pair_t<T, U> = 0>
[[nodiscard]] constexpr bool cmp_greater(T lhs, U rhs) noexcept {
  return cmp_less(rhs, lhs);
}

template <typename T, typename U, detail::require_integral_pair_t<T, U> = 0>
[[nodiscard]] constexpr bool cmp_less_equal(T lhs, U rhs) noexcept {
  return !cmp_less(rhs, lhs);
}

template <typename T, typename U, detail::require_integral_pair_t<T, U> = 0>
[[nodiscard]] constexpr bool cmp_greater_equal(T lhs, U rhs) noexcept {
  return !cmp_less(lhs, rhs);
}

template <typename R, typename T, detail::require_integral_t<R> = 0, detail::require_integral_t<T> = 0>
[[nodiscard]] constexpr bool in_range(T value) noexcept {
  return cmp_greater_equal(value, (std::numeric_limits<R>::min)()) && cmp_less_equal(value, (std::numeric_limits<R>::max)());
}

// https://artificial-mind.net/blog/2020/10/31/constexpr-for
template <auto Start, auto End, auto Inc, typename F, std::enable_if_t<detail::constexpr_for_traits<Start, End, Inc, F>::value, int> = 0>
constexpr void constexpr_for(F&& f) {
  if constexpr (Start < End) {
    static_cast<void>(f(std::integral_constant<decltype(Start), Start>{}));
    if constexpr (detail::can_increment_constexpr_for(Start, Inc)) {
      constexpr auto Next = static_cast<decltype(Start)>(Start + Inc);
      ::nstd::constexpr_for<Next, End, Inc>(std::forward<F>(f));
    }
  }
}

} // namespace nstd

#endif // NEARGYE_NSTD_UTILITY_HPP
