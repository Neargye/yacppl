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

#ifndef NEARGYE_NSTD_CONCEPTS_HPP
#define NEARGYE_NSTD_CONCEPTS_HPP

#include <cstddef>
#include <type_traits>

namespace nstd {

namespace detail {

template <typename T>
struct is_null_pointer : std::is_same<typename std::remove_cv<T>::type, std::nullptr_t> {};

} // namespace nstd::detail

template <bool C, typename T = void>
using EnableIf = typename std::enable_if<C, T>::type;

template <typename T, bool C>
using TypeConcept = EnableIf<C, T>;

template <typename T>
using RValue = TypeConcept<T, std::is_rvalue_reference<T&&>::value && !std::is_const<typename std::remove_reference<T>::type>::value>&&;

template <typename T>
using LValue = TypeConcept<T, std::is_lvalue_reference<T&&>::value>;

template <typename T>
using Reference = TypeConcept<T, std::is_reference<T>::value>;

template <typename T>
using Const = TypeConcept<T, std::is_const<typename std::remove_reference<T>::type>::value>;

template <typename T>
using NotConst = TypeConcept<T, !std::is_const<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Volatile = TypeConcept<T, std::is_volatile<typename std::remove_reference<T>::type>::value>;

template <typename T>
using NotVolatile = TypeConcept<T, !std::is_volatile<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Void = TypeConcept<T, std::is_void<typename std::remove_reference<T>::type>::value>;

template <typename T>
using NullPointer = TypeConcept<T, detail::is_null_pointer<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Arithmetic = TypeConcept<T, std::is_arithmetic<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Integral = TypeConcept<T, std::is_integral<typename std::remove_reference<T>::type>::value>;

template <typename T>
using SignedIntegral = TypeConcept<T, std::is_integral<typename std::remove_reference<T>::type>::value &&
                                      std::is_signed<typename std::remove_reference<T>::type>::value>;

template <typename T>
using UnsignedIntegral = TypeConcept<T, std::is_integral<typename std::remove_reference<T>::type>::value &&
                                        std::is_unsigned<typename std::remove_reference<T>::type>::value>;

template <typename T>
using FloatingPoint = TypeConcept<T, std::is_floating_point<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Enum = TypeConcept<T, std::is_enum<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Pointer = TypeConcept<T, std::is_pointer<typename std::remove_reference<T>::type>::value>;

template <typename T>
using MemberPointer = TypeConcept<T, std::is_member_pointer<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Array = TypeConcept<T, std::is_array<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Function = TypeConcept<T, std::is_function<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Object = TypeConcept<T, std::is_object<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Scalar = TypeConcept<T, std::is_scalar<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Class = TypeConcept<T, std::is_class<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Union = TypeConcept<T, std::is_union<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Trivial = TypeConcept<T, std::is_trivial<typename std::remove_reference<T>::type>::value>;

template <typename T>
using TriviallyCopyable = TypeConcept<T, std::is_trivially_copyable<typename std::remove_reference<T>::type>::value>;

template <typename T, typename... Args>
using Constructible = TypeConcept<T, std::is_constructible<T, Args...>::value>;

template <typename T, typename... Args>
using NothrowConstructible = TypeConcept<T, std::is_nothrow_constructible<T, Args...>::value>;

template <typename T>
using DefaultConstructible = TypeConcept<T, std::is_default_constructible<T>::value>;

template <typename T>
using CopyConstructible = TypeConcept<T, std::is_copy_constructible<T>::value>;

template <typename T>
using MoveConstructible = TypeConcept<T, std::is_move_constructible<T>::value>;

template <typename T>
using Destructible = TypeConcept<T, std::is_destructible<T>::value>;

template <typename T>
using NothrowDestructible = TypeConcept<T, std::is_nothrow_destructible<T>::value>;

template <typename T, typename U>
using Assignable = TypeConcept<T, std::is_assignable<T, U>::value>;

template <typename T, typename U>
using NothrowAssignable = TypeConcept<T, std::is_nothrow_assignable<T, U>::value>;

template <typename T>
using CopyAssignable = TypeConcept<T, std::is_copy_assignable<T>::value>;

template <typename T>
using MoveAssignable = TypeConcept<T, std::is_move_assignable<T>::value>;

template <typename T, typename U>
using Same = TypeConcept<T, std::is_same<T, U>::value>;

template <typename T, typename To>
using ConvertibleTo = TypeConcept<T, std::is_convertible<T, To>::value>;

} // namespace nstd

#endif // NEARGYE_NSTD_CONCEPTS_HPP
