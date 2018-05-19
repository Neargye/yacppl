//  _    _                          _    _____
// | |  | |                        | |  / ____|_     _
// | |  | |_ __  _   _ ___  ___  __| | | |   _| |_ _| |_
// | |  | | '_ \| | | / __|/ _ \/ _` | | |  |_   _|_   _|
// | |__| | | | | |_| \__ \  __/ (_| | | |____|_|   |_|
//  \____/|_| |_|\__,_|___/\___|\__,_|  \_____|
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// Copyright (c) 2018 Daniil Goncharov <neargye@gmail.com>.
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

#pragma once

namespace unused {

template <typename... Args>
inline void Unused(const Args&...) noexcept {}

} // namespace unused

#if defined(_MSC_VER) && _MSC_VER < 1910
// UNUSED macros with varying number of arguments to avoid "unused variable" warnings.
#  define UNUSED(...) ((void)(__VA_ARGS__))
#else
// UNUSED macros with varying number of arguments to avoid "unused variable" warnings.
#  define UNUSED(...) ((void)(decltype(::unused::Unused(__VA_ARGS__))()))
#endif
