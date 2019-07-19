//  _    _                          _    _____
// | |  | |                        | |  / ____|_     _
// | |  | |_ __  _   _ ___  ___  __| | | |   _| |_ _| |_
// | |  | | '_ \| | | / __|/ _ \/ _` | | |  |_   _|_   _|
// | |__| | | | | |_| \__ \  __/ (_| | | |____|_|   |_|
//  \____/|_| |_|\__,_|___/\___|\__,_|  \_____|
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

#ifndef NEARGYE_UNUSED_HPP
#define NEARGYE_UNUSED_HPP

namespace unused {

// Function with varying number of arguments to avoid "unused variable" warnings.
template <typename... Args>
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
constexpr
#else
inline
#endif
void unused_variable(const Args&...) noexcept {}

} // namespace unused

#if defined(_MSC_VER)
// Macro with varying number of arguments to avoid "unused variable" warnings.
#  define UNUSED(...) ((void)(__VA_ARGS__));
#else
// Macro with varying number of arguments to avoid "unused variable" warnings.
#  define UNUSED(...) (decltype(::unused::unused_variable(__VA_ARGS__))());
#endif

#endif // NEARGYE_UNUSED_HPP
