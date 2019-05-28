//  _   _  _____ _______ _____     _____
// | \ | |/ ____|__   __|  __ \   / ____|_     _
// |  \| | (___    | |  | |  | | | |   _| |_ _| |_
// | . ` |\___ \   | |  | |  | | | |  |_   _|_   _|
// | |\  |____) |  | |  | |__| | | |____|_|   |_|
// |_| \_|_____/   |_|  |_____/   \_____|
// vesion 0.3.1
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2019 Daniil Goncharov <neargye@gmail.com>.
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

#ifndef NEARGYE_NSTD_HPP
#define NEARGYE_NSTD_HPP

#include <type_traits>

namespace nstd {

template <typename T>
struct identity {
  using type = T;
};

template <typename T>
using identity_t = typename identity<T>::type;

// Removes all pointer from the given type.
template <typename T>
struct remove_all_p
    : std::conditional<std::is_pointer<T>::value,
          remove_all_p<typename std::remove_pointer<T>::type>,
          identity<T>
              >::type {};

template <typename T>
using remove_all_p_t = typename remove_all_p<T>::type;

// Removes const, volatile, reference specifiers from the given type.
template <typename T>
struct remove_cvr
    : std::remove_cv<typename std::remove_reference<T>::type> {};

template <typename T>
using remove_cvr_t = typename remove_cvr<T>::type;

// Removes all const, volatile, reference, pointer specifiers from the given type.
template <typename T>
struct remove_all_cvrp
    : remove_cvr<typename remove_all_p<typename remove_cvr<T>::type>::type> {};

template <typename T>
using remove_all_cvrp_t = typename remove_all_cvrp<T>::type;

// Removes all const, volatile, reference, pointer, array extents specifiers from the given type.
template <typename T, typename U = typename remove_all_cvrp<T>::type>
struct remove_all_cvrpe
    : std::conditional<std::is_array<U>::value,
          remove_all_cvrpe<typename std::remove_all_extents<U>::type>,
          identity<U>
              >::type {};

template <typename T>
using remove_all_cvrpe_t = typename remove_all_cvrpe<T>::type;

template <typename>
struct is_template
    : std::false_type {};

template <template <typename...> class T, typename... A>
struct is_template<T<A...>>
    : std::true_type {};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <typename T>
#  if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#  endif
constexpr bool is_template_v = is_template<T>::value;
#endif

template <typename>
struct template_nargs
    : std::integral_constant<std::size_t, 0> {};

template <template <typename...> class T, typename... A>
struct template_nargs<T<A...>>
    : std::integral_constant<std::size_t, sizeof...(A)> {};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <typename T>
#  if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#  endif
constexpr typename template_nargs<T>::value_type template_nargs_v = template_nargs<T>::value;
#endif

// Checks if two types are the same signed/unsigned.
template <typename T, typename U>
struct is_same_signed
    : std::integral_constant<bool, std::is_signed<T>::value == std::is_signed<U>::value &&
                                       std::is_unsigned<T>::value == std::is_unsigned<U>::value> {};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
template <typename T, typename U>
#  if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
inline
#  endif
constexpr bool is_same_signed_v = is_same_signed<T, U>::value;
#endif

template <typename T>
constexpr typename std::conditional<!std::is_nothrow_move_assignable<T>::value &&
                                        std::is_copy_assignable<T>::value,
                                    const T&, T&&>::type
move_assign_if_noexcept(T& x) noexcept {
  return static_cast<typename std::remove_reference<T>::type&&>(x);
}

} // namespace nstd

#endif // NEARGYE_NSTD_HPP
