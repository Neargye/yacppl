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

#ifndef NEARGYE_NSTD_TYPE_TRAITS_HPP
#define NEARGYE_NSTD_TYPE_TRAITS_HPP

#include <type_traits>

namespace nstd {

namespace detail {

template <typename... T>
struct void_t {
  using type = void;
};

template <typename Default, typename AlwaysVoid, template <typename...> class Op, typename... Args>
struct detector {
  using type = Default;
  using value_t = std::false_type;
};

template <typename Default, template <typename...> class Op, typename... Args>
struct detector<Default, typename void_t<Op<Args...>>::type, Op, Args...> {
  using type = Op<Args...>;
  using value_t = std::true_type;
};

struct nonesuch final {
  nonesuch() = delete;
  nonesuch(const nonesuch&) = delete;
  void operator=(const nonesuch&) = delete;
  ~nonesuch() = delete;
};

} // namespace nstd::detail

template <typename... T>
using void_t = typename detail::void_t<T...>::type;

template <typename Default, template <typename...> class Op, typename... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;

template <typename Default, template <typename...> class Op, typename... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template <template <typename...> class Op, typename... Args>
using is_detected = typename detail::detector<detail::nonesuch, void, Op, Args...>::value_t;

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <template <typename...> class Op, typename... Args>
#  if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#  endif
constexpr bool is_detected_v = detail::detector<detail::nonesuch, void, Op, Args...>::value_t::value;
#endif

template <template <typename...> class Op, typename... Args>
using detected_t = typename detail::detector<detail::nonesuch, void, Op, Args...>::type;

template <typename Expected, template <typename...> class Op, typename... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <typename Expected, template <typename...> class Op, typename... Args>
#  if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#  endif
constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;
#endif

template <typename To, template <typename...> class Op, typename... Args>
using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <typename To, template <typename...> class Op, typename... Args>
#  if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#  endif
constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;
#endif

template <typename T>
struct identity {
  using type = T;
};

template <typename T>
using identity_t = typename identity<T>::type;

// Removes pointer specifiers from the given type.
template <typename T>
struct remove_ptr : std::remove_pointer<T> {};

template <typename T>
using remove_ptr_t = typename std::remove_pointer<T>::type;

// Removes reference specifiers from the given type.
template <typename T>
struct remove_ref : std::remove_reference<T> {};

template <typename T>
using remove_ref_t = typename std::remove_reference<T>::type;

// Removes const, volatile, reference specifiers from the given type.
template <typename T>
struct remove_cv_ref : std::remove_cv<typename std::remove_reference<T>::type> {};

template <typename T>
using remove_cv_ref_t = typename remove_cv_ref<T>::type;

// Removes all pointer from the given type.
template <typename T>
struct remove_all_ptr
    : std::conditional<std::is_pointer<T>::value,
          remove_all_ptr<typename std::remove_pointer<T>::type>,
          identity<T>
              >::type {};

template <typename T>
using remove_all_ptr_t = typename remove_all_ptr<T>::type;

// Removes all const, volatile, reference, pointer specifiers from the given type.
template <typename T>
struct remove_all_cv_ref_ptr : remove_cv_ref<typename remove_all_ptr<typename remove_cv_ref<T>::type>::type> {};

template <typename T>
using remove_all_cv_ref_ptr_t = typename remove_all_cv_ref_ptr<T>::type;

// Removes all const, volatile, reference, pointer, array extents specifiers from the given type.
template <typename T>
struct remove_all_cv_ref_ptr_ext
    : std::conditional<std::is_array<typename remove_all_cv_ref_ptr<T>::type>::value,
          remove_all_cv_ref_ptr_ext<typename std::remove_all_extents<typename remove_all_cv_ref_ptr<T>::type>::type>,
          identity<typename remove_all_cv_ref_ptr<T>::type>
              >::type {};

template <typename T>
using remove_all_cv_ref_ptr_ext_t = typename remove_all_cv_ref_ptr_ext<T>::type;

// Checks if two types are the same signed/unsigned.
template <typename T, typename U>
struct is_same_signedness : std::integral_constant<bool, std::is_signed<T>::value == std::is_signed<U>::value &&
                                                         std::is_unsigned<T>::value == std::is_unsigned<U>::value> {};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <typename T, typename U>
#  if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#  endif
constexpr bool is_same_signedness_v = is_same_signedness<T, U>::value;
#endif

#if defined(__cpp_lib_is_nothrow_convertible) && __cpp_lib_is_nothrow_convertible >= 201806L
template <typename From, typename To>
struct is_nothrow_convertible : std::is_nothrow_convertible<From, To> {};

#  if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <typename From, typename To>
#    if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#    endif
constexpr bool is_nothrow_convertible_v = std::is_nothrow_convertible<From, To>::value;
#  endif
#else
namespace detail {
// https://reviews.llvm.org/D58019
template <typename From, typename To,
          bool = std::integral_constant<bool, std::is_void<From>::value ||
                                              std::is_function<To>::value ||
                                              std::is_array<To>::value>::value>
struct is_nothrow_convertible {
  using type = std::is_void<To>;
};

struct is_nothrow_convertible_impl {
  template <typename To>
  static void test_aux(To) noexcept;

  template <typename From, typename To>
  static std::integral_constant<bool, noexcept(test_aux<To>(std::declval<From>()))> test(int);

  template <typename, typename>
  static std::false_type test(...);
};

template <typename From, typename To>
struct is_nothrow_convertible<From, To, false> {
  using type = decltype(is_nothrow_convertible_impl::test<From, To>(0));
};

} // nstd::detail

template <typename From, typename To>
struct is_nothrow_convertible
    : detail::is_nothrow_convertible<From, To>::type {};

#  if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <typename From, typename To>
#    if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#    endif
constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;
#  endif
#endif

} // namespace nstd

#endif // NEARGYE_NSTD_TYPE_TRAITS_HPP
