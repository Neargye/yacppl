//          _   _        _ _           _               _____
//     /\  | | | |      (_) |         | |             / ____|_     _
//    /  \ | |_| |_ _ __ _| |__  _   _| |_ ___  ___  | |   _| |_ _| |_
//   / /\ \| __| __| '__| | '_ \| | | | __/ _ \/ __| | |  |_   _|_   _|
//  / ____ \ |_| |_| |  | | |_) | |_| | ||  __/\__ \ | |____|_|   |_|
// /_/    \_\__|\__|_|  |_|_.__/ \__,_|\__\___||___/  \_____|
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

// CPP_ATTR_NORETURN indicates that the function does not return.
#if !defined(CPP_ATTR_NORETURN)
#  if defined(_MSC_VER)
#    if (_MSC_FULL_VER >= 190024210)
#      define CPP_ATTR_NORETURN [[noreturn]]
#    else
#      define CPP_ATTR_NORETURN __declspec(noreturn)
#    endif
#  elif defined(__clang__)
#    if ((__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 3)) && __cplusplus >= 201103L)
#      define CPP_ATTR_NORETURN [[noreturn]]
#    else
#      define CPP_ATTR_NORETURN __attribute__((__noreturn__))
#    endif
#  elif defined(__GNUC__)
#    if ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)) && __cplusplus >= 201103L)
#      define CPP_ATTR_NORETURN [[noreturn]]
#    else
#      define CPP_ATTR_NORETURN __attribute__((__noreturn__))
#    endif
#  else
#    define CPP_ATTR_NORETURN
#  endif
#endif

// TODO: CPP_ATTR_CARRIES_DEPENDENCY

// TODO: CPP_ATTR_DEPRECATED(reason)

// CPP_ATTR_DEPRECATED indicates that the use of the name or entity declared with this attribute is allowed, but discouraged for some reason.
#if !defined(CPP_ATTR_DEPRECATED)
#  if defined(_MSC_VER)
#    if (_MSC_VER >= 1900)
#      define CPP_ATTR_DEPRECATED [[deprecated]]
#    else
#      define CPP_ATTR_DEPRECATED __declspec(deprecated)
#    endif
#  elif defined(__clang__)
#    if ((__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 4)) && __cplusplus >= 201402L)
#      define CPP_ATTR_DEPRECATED [[deprecated]]
#    else
#      define CPP_ATTR_DEPRECATED __attribute__((__deprecated__))
#    endif
#  elif defined(__GNUC__)
#    if ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)) && __cplusplus >= 201402L)
#      define CPP_ATTR_DEPRECATED [[deprecated]]
#    else
#      define CPP_ATTR_DEPRECATED __attribute__((__deprecated__))
#    endif
#  else
#    define CPP_ATTR_DEPRECATED
#  endif
#endif

// CPP_ATTR_FALLTHROUGH indicates that the fall through from the previous case label is intentional and should not be diagnosed by a compiler that warns on fall-through.
#if !defined(CPP_ATTR_FALLTHROUGH)
#  if defined(_MSC_VER)
#    if (_MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#      define CPP_ATTR_FALLTHROUGH [[fallthrough]]
#    else
#      define CPP_ATTR_FALLTHROUGH
#    endif
#  elif defined(__clang__)
#    if ((__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 9)) && __cplusplus >= 201703L)
#      define CPP_ATTR_FALLTHROUGH [[fallthrough]]
#    else
#      define CPP_ATTR_FALLTHROUGH
#    endif
#  elif defined(__GNUC__)
#    if (__GNUC__ > 7 && __cplusplus >= 201703L)
#      define CPP_ATTR_FALLTHROUGH [[fallthrough]]
#    else
#      define CPP_ATTR_FALLTHROUGH
#    endif
#  else
#    define CPP_ATTR_FALLTHROUGH
#  endif
#endif

// CPP_ATTR_NODISCARD encourages the compiler to issue a warning if the return value is discarded.
#if !defined(CPP_ATTR_NODISCARD)
#  if defined(_MSC_VER)
#    if (_MSC_VER >= 1911 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#      define CPP_ATTR_NODISCARD [[nodiscard]]
#    elif defined(_Check_return_)
#      define CPP_ATTR_NODISCARD _Check_return_
#    else
#      define CPP_ATTR_NODISCARD
#    endif
#  elif defined(__clang__)
#    if ((__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 9)) && __cplusplus >= 201703L)
#      define CPP_ATTR_NODISCARD [[nodiscard]]
#    else
#      define CPP_ATTR_NODISCARD __attribute__((__warn_unused_result__))
#    endif
#  elif defined(__GNUC__)
#    if (__GNUC__ > 7 && __cplusplus >= 201703L)
#      define CPP_ATTR_NODISCARD [[nodiscard]]
#    else
#      define CPP_ATTR_NODISCARD __attribute__((__warn_unused_result__))
#    endif
#  else
#    define CPP_ATTR_NODISCARD
#  endif
#endif

// CPP_ATTR_MAYBE_UNUSED suppresses compiler warnings on unused entities, if any.
#if !defined(CPP_ATTR_MAYBE_UNUSED)
#  if defined(_MSC_VER)
#    if (_MSC_VER >= 1911 && defined(_MSVC_LANG) &&_MSVC_LANG >= 201703L)
#      define CPP_ATTR_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define CPP_ATTR_MAYBE_UNUSED __pragma(warning(suppress : 4100 4101 4189))
#    endif
#  elif defined(__clang__)
#    if ((__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 9)) && __cplusplus >= 201703L)
#      define CPP_ATTR_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define CPP_ATTR_MAYBE_UNUSED __attribute__((__unused__))
#    endif
#  elif defined(__GNUC__)
#    if (__GNUC__ > 7 && __cplusplus >= 201703L)
#      define CPP_ATTR_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define CPP_ATTR_MAYBE_UNUSED __attribute__((__unused__))
#    endif
#  else
#    define CPP_ATTR_MAYBE_UNUSED
#  endif
#endif

// TODO: CPP_ATTR_LIKELY
// TODO: CPP_ATTR_CARRIES_UNLIKELY
// TODO: CPP_ATTR_CARRIES_NO_UNIQUE_ADDRESS
