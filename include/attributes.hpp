//          _   _        _ _           _               _____
//     /\  | | | |      (_) |         | |             / ____|_     _
//    /  \ | |_| |_ _ __ _| |__  _   _| |_ ___  ___  | |   _| |_ _| |_
//   / /\ \| __| __| '__| | '_ \| | | | __/ _ \/ __| | |  |_   _|_   _|
//  / ____ \ |_| |_| |  | | |_) | |_| | ||  __/\__ \ | |____|_|   |_|
// /_/    \_\__|\__|_|  |_|_.__/ \__,_|\__\___||___/  \_____|
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

// ATTR_NORETURN indicates that the function does not return.
#if !defined(ATTR_NORETURN)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 33 && __cplusplus >= 201103L
#      define ATTR_NORETURN [[noreturn]]
#    else
#      define ATTR_NORETURN __attribute__((__noreturn__))
#    endif
#  elif defined(__GNUC__)
#    if (__GNUC__ * 10 + __GNUC_MINOR__) >= 48 && __cplusplus >= 201103L
#      define ATTR_NORETURN [[noreturn]]
#    else
#      define ATTR_NORETURN __attribute__((__noreturn__))
#    endif
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1910
#      define ATTR_NORETURN [[noreturn]]
#    else
#      define ATTR_NORETURN __declspec(noreturn)
#    endif
#  else
#    define ATTR_NORETURN
#  endif
#endif

// TODO: ATTR_CARRIES_DEPENDENCY

// TODO: ATTR_DEPRECATED no msd

// ATTR_DEPRECATED indicates that the use of the name or entity declared with this attribute is allowed, but discouraged for some reason.
#if !defined(ATTR_DEPRECATED)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 34 && __cplusplus >= 201402L
#      define ATTR_DEPRECATED(MSG) [[deprecated(MSG)]]
#    else
#      define ATTR_DEPRECATED(MSG) __attribute__((__deprecated__(MSG)))
#    endif
#  elif defined(__GNUC__)
#    if (__GNUC__ * 10 +  __GNUC_MINOR__) >= 49 && __cplusplus >= 201402L
#      define ATTR_DEPRECATED(MSG) [[deprecated(MSG)]]
#    else
#      define ATTR_DEPRECATED(MSG) __attribute__((__deprecated__(MSG)))
#    endif
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1900
#      define ATTR_DEPRECATED(MSG) [[deprecated(MSG)]]
#    else
#      define ATTR_DEPRECATED(MSG) __declspec(deprecated(MSG))
#    endif
#  else
#    define ATTR_DEPRECATED(MSG)
#  endif
#endif

// ATTR_FALLTHROUGH indicates that the fall through from the previous case label is intentional and should not be diagnosed by a compiler that warns on fall-through.
#if !defined(ATTR_FALLTHROUGH)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 39 && __cplusplus >= 201703L
#      define ATTR_FALLTHROUGH [[fallthrough]];
#    else
#      define ATTR_FALLTHROUGH [[clang::fallthrough]];
#    endif
#  elif defined(__GNUC__)
#    if __GNUC__ >= 7 && __cplusplus >= 201703L
#      define ATTR_FALLTHROUGH [[fallthrough]];
#    else
#      define ATTR_FALLTHROUGH /*fallthrough*/
#    endif
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#      define ATTR_FALLTHROUGH [[fallthrough]];
#    else
#      define ATTR_FALLTHROUGH /*fallthrough*/
#    endif
#  else
#    define ATTR_FALLTHROUGH /*fallthrough*/
#  endif
#endif

// TODO: ATTR_NODISCARD with msd

// ATTR_NODISCARD encourages the compiler to issue a warning if the return value is discarded.
#if !defined(ATTR_NODISCARD)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 39 && __cplusplus >= 201703L
#      define ATTR_NODISCARD [[nodiscard]]
#    else
#      define ATTR_NODISCARD __attribute__((__warn_unused_result__))
#    endif
#  elif defined(__GNUC__)
#    if __GNUC__ >= 7 && __cplusplus >= 201703L
#      define ATTR_NODISCARD [[nodiscard]]
#    else
#      define ATTR_NODISCARD __attribute__((__warn_unused_result__))
#    endif
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1911 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#      define ATTR_NODISCARD [[nodiscard]]
#    elif defined(_Check_return_)
#      define ATTR_NODISCARD _Check_return_
#    else
#      define ATTR_NODISCARD
#    endif
#  else
#    define ATTR_NODISCARD
#  endif
#endif

// ATTR_MAYBE_UNUSED suppresses compiler warnings on unused entities, if any.
#if !defined(ATTR_MAYBE_UNUSED)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 39 && __cplusplus >= 201703L
#      define ATTR_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define ATTR_MAYBE_UNUSED __attribute__((__unused__))
#    endif
#  elif defined(__GNUC__)
#    if __GNUC__ >= 7 && __cplusplus >= 201703L
#      define ATTR_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define ATTR_MAYBE_UNUSED __attribute__((__unused__))
#    endif
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1911 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#      define ATTR_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define ATTR_MAYBE_UNUSED __pragma(warning(suppress : 4100 4101 4189))
#    endif
#  else
#    define ATTR_MAYBE_UNUSED
#  endif
#endif

// TODO: [[likely]]
// ATTR_LIKELY indicates that the compiler should optimize for the case where a path of execution through a statement is more or less likely than any other path of execution.
#if !defined(ATTR_LIKELY)
#  if defined(__clang__) || defined(__GNUC__)
#    define ATTR_LIKELY(x) __builtin_expect(!!(x), 1)
#  else
#    define ATTR_LIKELY(x) (!!(x))
#  endif
#endif

// TODO: [[unlikely]]
// ATTR_UNLIKELY indicates that the compiler should optimize for the case where a path of execution through a statement is more or less likely than any other path of execution.
#if !defined(ATTR_UNLIKELY)
#  if defined(__clang__) || defined(__GNUC__)
#    define ATTR_UNLIKELY(x) __builtin_expect(!!(x), 0)
#  else
#    define ATTR_UNLIKELY(x) (!!(x))
#  endif
#endif

// TODO: ATTR_NO_UNIQUE_ADDRESS
// https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/#msvc-extensions-and-abi
// https://github.com/microsoft/STL/issues/1364
