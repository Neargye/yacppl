//          _   _        _ _           _               _____
//     /\  | | | |      (_) |         | |             / ____|_     _
//    /  \ | |_| |_ _ __ _| |__  _   _| |_ ___  ___  | |   _| |_ _| |_
//   / /\ \| __| __| '__| | '_ \| | | | __/ _ \/ __| | |  |_   _|_   _|
//  / ____ \ |_| |_| |  | | |_) | |_| | ||  __/\__ \ | |____|_|   |_|
// /_/    \_\__|\__|_|  |_|_.__/ \__,_|\__\___||___/  \_____|
// vesion 0.1.3
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

#pragma once

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
// TODO: ATTR_DEPRECATED(reason)

// ATTR_DEPRECATED indicates that the use of the name or entity declared with this attribute is allowed, but discouraged for some reason.
#if !defined(ATTR_DEPRECATED)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 34 && __cplusplus >= 201402L
#      define ATTR_DEPRECATED [[deprecated]]
#    else
#      define ATTR_DEPRECATED __attribute__((__deprecated__))
#    endif
#  elif defined(__GNUC__)
#    if (__GNUC__ * 10 +  __GNUC_MINOR__) >= 49 && __cplusplus >= 201402L
#      define ATTR_DEPRECATED [[deprecated]]
#    else
#      define ATTR_DEPRECATED __attribute__((__deprecated__))
#    endif
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1900
#      define ATTR_DEPRECATED [[deprecated]]
#    else
#      define ATTR_DEPRECATED __declspec(deprecated)
#    endif
#  else
#    define ATTR_DEPRECATED
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
#      define ATTR_FALLTHROUGH
#    endif
#  else
#    define ATTR_FALLTHROUGH
#  endif
#endif

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

// TODO: ATTR_LIKELY
// TODO: ATTR_UNLIKELY
// TODO: ATTR_NO_UNIQUE_ADDRESS
