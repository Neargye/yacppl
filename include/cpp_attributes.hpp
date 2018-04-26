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

#if !defined(CPP_HAS_ATTRIBUTE)
#  if defined(__has_cpp_attribute)
#    define CPP_HAS_ATTRIBUTE(x) __has_cpp_attribute(x)
#  else
#    define CPP_HAS_ATTRIBUTE(x) 0
#  endif
#endif

// CPP_ATTRIBUTE_UNUSED indicates that a function, variable or parameter might or might not be used.
#if !defined(CPP_ATTRIBUTE_UNUSED)
#  if defined(_MSC_VER)
#    if CPP_HAS_ATTRIBUTE(maybe_unused) || (_MSC_VER >= 1911 && _HAS_CXX17)
#      define CPP_ATTRIBUTE_UNUSED [[maybe_unused]]
#    else
#      define CPP_ATTRIBUTE_UNUSED __pragma(warning(suppress : 4100 4101 4189))
#    endif
#  elif defined(__GNUC__) || defined(__clang__)
#    if (__cplusplus >= 201703L) && CPP_HAS_ATTRIBUTE(maybe_unused)
#      define CPP_ATTRIBUTE_UNUSED [[maybe_unused]]
#    else
#      define CPP_ATTRIBUTE_UNUSED __attribute__((unused))
#    endif
#  else
#    define CPP_ATTRIBUTE_UNUSED
#  endif
#endif
