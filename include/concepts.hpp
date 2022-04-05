//  _   _  _____ _______ _____     _____
// | \ | |/ ____|__   __|  __ \   / ____|_     _
// |  \| | (___    | |  | |  | | | |   _| |_ _| |_
// | . ` |\___ \   | |  | |  | | | |  |_   _|_   _|
// | |\  |____) |  | |  | |__| | | |____|_|   |_|
// |_| \_|_____/   |_|  |_____/   \_____|
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2022 Daniil Goncharov <neargye@gmail.com>.
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

#include <type_traits>

namespace nstd {

namespace detail {

template <typename T, typename Enable = void>
using type_concept = T;

} // namespace nstd::detail

template <typename T, bool C>
using TypeConcept = detail::type_concept<T, typename std::enable_if<C>::type>;

template <typename T>
using RValue = TypeConcept<T, std::is_rvalue_reference<T&&>::value && !std::is_const<typename std::remove_reference<T>::type>::value>&&;

template <typename T>
using Const = TypeConcept<T, std::is_const<typename std::remove_reference<T>::type>::value>;

template <typename T>
using NotConst = TypeConcept<T, !std::is_const<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Arithmetic = TypeConcept<T, std::is_arithmetic<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Integral = TypeConcept<T, std::is_integral<typename std::remove_reference<T>::type>::value>;

template <typename T>
using Trivial = TypeConcept<T, std::is_trivial<typename std::remove_reference<T>::type>::value>;

template <typename T>
using TriviallyCopyable = TypeConcept<T, std::is_trivially_copyable<typename std::remove_reference<T>::type>::value>;

} // namespace nstd

#endif // NEARGYE_NSTD_CONCEPTS_HPP
