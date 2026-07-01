//          _   _        _ _           _               _____
//     /\  | | | |      (_) |         | |             / ____|_     _
//    /  \ | |_| |_ _ __ _| |__  _   _| |_ ___  ___  | |   _| |_ _| |_
//   / /\ \| __| __| '__| | '_ \| | | | __/ _ \/ __| | |  |_   _|_   _|
//  / ____ \ |_| |_| |  | | |_) | |_| | ||  __/\__ \ | |____|_|   |_|
// /_/    \_\__|\__|_|  |_|_.__/ \__,_|\__\___||___/  \_____|
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

#ifndef NEARGYE_NSTD_ATTRIBUTES_HPP
#define NEARGYE_NSTD_ATTRIBUTES_HPP

#if !defined(NEARGYE_ATTR_HAS_CPP_ATTRIBUTE)
#  if defined(__has_cpp_attribute)
#    define NEARGYE_ATTR_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#  else
#    define NEARGYE_ATTR_HAS_CPP_ATTRIBUTE(x) 0
#  endif
#  define NEARGYE_ATTR_HAS_CPP_ATTRIBUTE_DEFINED
#endif

#if !defined(NEARGYE_ATTR_HAS_BUILTIN)
#  if defined(__has_builtin)
#    define NEARGYE_ATTR_HAS_BUILTIN(x) __has_builtin(x)
#  else
#    define NEARGYE_ATTR_HAS_BUILTIN(x) 0
#  endif
#  define NEARGYE_ATTR_HAS_BUILTIN_DEFINED
#endif

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

// ATTR_ALWAYS_INLINE strongly suggests that a function should be inlined.
#if !defined(ATTR_ALWAYS_INLINE)
#  if defined(_MSC_VER)
#    define ATTR_ALWAYS_INLINE __forceinline
#  elif defined(__clang__) || defined(__GNUC__)
#    define ATTR_ALWAYS_INLINE inline __attribute__((__always_inline__))
#  else
#    define ATTR_ALWAYS_INLINE inline
#  endif
#endif

// ATTR_FALLTHROUGH indicates that the fall through from the previous case label is intentional and should not be diagnosed by a compiler that warns on fall-through.
#if !defined(ATTR_FALLTHROUGH)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 39 && __cplusplus >= 201703L
#      define ATTR_FALLTHROUGH [[fallthrough]];
#    elif __cplusplus >= 201103L
#      define ATTR_FALLTHROUGH [[clang::fallthrough]];
#    else
#      define ATTR_FALLTHROUGH /*fallthrough*/
#    endif
#  elif defined(__GNUC__)
#    if __GNUC__ >= 7 && __cplusplus >= 201703L
#      define ATTR_FALLTHROUGH [[fallthrough]];
#    elif __GNUC__ >= 7 && __cplusplus >= 201103L
#      define ATTR_FALLTHROUGH [[gnu::fallthrough]];
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

// ATTR_ASSUME gives the optimizer an assumption about an expression. If the expression is false, behavior is undefined on supporting compilers.
#if !defined(ATTR_ASSUME)
#  if ((defined(_MSVC_LANG) && _MSVC_LANG >= 202302L) || __cplusplus >= 202302L) && NEARGYE_ATTR_HAS_CPP_ATTRIBUTE(assume)
#    define ATTR_ASSUME(EXPR) [[assume(EXPR)]]
#  elif defined(_MSC_VER)
#    define ATTR_ASSUME(EXPR) __assume(EXPR)
#  elif NEARGYE_ATTR_HAS_BUILTIN(__builtin_assume)
#    define ATTR_ASSUME(EXPR) __builtin_assume(EXPR)
#  else
#    define ATTR_ASSUME(EXPR) static_cast<void>(0)
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

// ATTR_NODISCARD_MSG encourages the compiler to issue a warning with a reason if the return value is discarded.
#if !defined(ATTR_NODISCARD_MSG)
#  if ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L) && NEARGYE_ATTR_HAS_CPP_ATTRIBUTE(nodiscard) >= 201907L
#    define ATTR_NODISCARD_MSG(MSG) [[nodiscard(MSG)]]
#  else
#    define ATTR_NODISCARD_MSG(MSG) ATTR_NODISCARD
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

// ATTR_LIKELY indicates that the compiler should optimize for the case where a path of execution through a statement is more or less likely than any other path of execution.
#if !defined(ATTR_LIKELY)
#  if defined(__clang__) || defined(__GNUC__)
#    define ATTR_LIKELY(x) __builtin_expect(!!(x), 1)
#  else
#    define ATTR_LIKELY(x) (!!(x))
#  endif
#endif

// ATTR_UNLIKELY indicates that the compiler should optimize for the case where a path of execution through a statement is more or less likely than any other path of execution.
#if !defined(ATTR_UNLIKELY)
#  if defined(__clang__) || defined(__GNUC__)
#    define ATTR_UNLIKELY(x) __builtin_expect(!!(x), 0)
#  else
#    define ATTR_UNLIKELY(x) (!!(x))
#  endif
#endif

// ATTR_TRIVIAL_ABI requests Clang's trivial_abi calling convention for eligible class types.
#if !defined(ATTR_TRIVIAL_ABI)
#  if defined(__clang__) && NEARGYE_ATTR_HAS_CPP_ATTRIBUTE(clang::trivial_abi)
#    define ATTR_TRIVIAL_ABI [[clang::trivial_abi]]
#  else
#    define ATTR_TRIVIAL_ABI
#  endif
#endif

// ATTR_NO_UNIQUE_ADDRESS indicates that a non-static data member need not have an address distinct from other non-static data members.
// MSVC uses a vendor spelling for the ABI-affecting implementation.
// https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/#msvc-extensions-and-abi
// https://github.com/microsoft/STL/issues/1364
#if !defined(ATTR_NO_UNIQUE_ADDRESS)
#  if ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L) && NEARGYE_ATTR_HAS_CPP_ATTRIBUTE(no_unique_address) >= 201803L
#    define ATTR_NO_UNIQUE_ADDRESS [[no_unique_address]]
#  elif defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L && NEARGYE_ATTR_HAS_CPP_ATTRIBUTE(msvc::no_unique_address)
#    define ATTR_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#  else
#    define ATTR_NO_UNIQUE_ADDRESS
#  endif
#endif

#if defined(NEARGYE_ATTR_HAS_CPP_ATTRIBUTE_DEFINED)
#  undef NEARGYE_ATTR_HAS_CPP_ATTRIBUTE
#  undef NEARGYE_ATTR_HAS_CPP_ATTRIBUTE_DEFINED
#endif

#if defined(NEARGYE_ATTR_HAS_BUILTIN_DEFINED)
#  undef NEARGYE_ATTR_HAS_BUILTIN
#  undef NEARGYE_ATTR_HAS_BUILTIN_DEFINED
#endif

#endif // NEARGYE_NSTD_ATTRIBUTES_HPP
