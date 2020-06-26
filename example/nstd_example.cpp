// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2020 Daniil Goncharov <neargye@gmail.com>.
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

#include <attributes.hpp>
#include <byte.hpp>
#include <concepts.hpp>
#include <type_traits.hpp>
#include <unused.hpp>
#include <utility.hpp>

#include <type_traits>
#include <string>
#include <iostream>

struct A {};

struct B {
  B() = default;
  ~B() = default;
  B(B&&) = default;
  B(const B&) = default;
  B& operator=(const B&) = default;
  B& operator=(B&&) = delete;
};

struct C {
  C() = default;
  ~C() = default;
  C(C&&) = default;
  C(const C&) = default;
  C& operator=(const C&) = delete;
  C& operator=(C&&) = default;
};

struct D {
  D() = default;
  ~D() = default;
  D(D&&) = default;
  D(const D&) = default;
  D& operator=(const D&) = default;
  D& operator=(D&&) noexcept(false) {
    return *this;
  }
};

using S = std::string;

template <typename T>
using copy_assign_t = decltype(std::declval<T&>() = std::declval<const T&>());

int main() {
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ > 5) || (defined(_MSC_VER) && _MSC_VER >= 1910)
  static_assert(std::is_same<nstd::void_t<>, void>::value, "");
  static_assert(nstd::is_detected<copy_assign_t, A>::value, "");
  static_assert(!nstd::is_detected<copy_assign_t, C>::value, "");
#  if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
  static_assert(nstd::is_detected_v<copy_assign_t, B>, "");
#  endif
#endif

  static_assert(std::is_same<nstd::remove_all_ptr<int**>::type, int>::value, "");

  int* const* volatile * p;
  NSTD_UNUSED(p);
  static_assert(std::is_same<nstd::remove_all_ptr_t<decltype(p)>, int>::value, "");

  const auto& rp = &p;
  NSTD_UNUSED(rp);
  static_assert(std::is_same<nstd::remove_all_cv_ref_ptr<decltype(rp)>::type, int>::value, "");

  const auto& rp_ = &rp;
  NSTD_UNUSED(rp_);
  static_assert(std::is_same<nstd::remove_all_cv_ref_ptr_t<decltype(rp_)>, int>::value, "");

  const volatile int* const* volatile**** i[10];
  NSTD_UNUSED(i);
  static_assert(std::is_same<nstd::remove_all_cv_ref_ptr_ext<decltype(i)>::type, int>::value, "");

  const auto& ri = &i;
  NSTD_UNUSED(ri);
  static_assert(std::is_same<nstd::remove_all_cv_ref_ptr_ext_t<decltype(ri)>, int>::value, "");

  static_assert(nstd::is_same_signedness<unsigned int, unsigned short>::value, "");
  static_assert(!nstd::is_same_signedness<unsigned int, signed short>::value, "");
#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
  static_assert(nstd::is_same_signedness_v<signed int, signed short>, "");
#endif

  A a;
  NSTD_UNUSED(a);
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(a)), A&&>::value, "");

  B b;
  NSTD_UNUSED(b);
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(b)), const B&>::value, "");

  C c;
  NSTD_UNUSED(c);
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(c)), C&&>::value, "");

  D d;
  NSTD_UNUSED(d);
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(d)), const D&>::value, "");

  nstd::invoke_each([](const auto& x) { std::cout << x << " "; }, "invoke_each test", 1, 2, 3);
  std::cout << std::endl;

  nstd::apply_each([](const auto& x) { std::cout << x << " "; }, std::make_tuple("apply_each test", 1, 2, 3));
  std::cout << std::endl;

  return 0;
}
