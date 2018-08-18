// nstd example
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

#include <nstd.hpp>
#include <type_traits>
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

int main() {
  static_assert(std::is_same<nstd::remove_all_pointers_t<int**>, int>::value, "what?");
  static_assert(std::is_same<nstd::remove_all_pointers_t<int* const* volatile *>, int>::value, "what?");

  static_assert(std::is_same<nstd::remove_all_cvrp_t<const volatile int* const* volatile****>, int>::value, "what?");

  static_assert(std::is_same<nstd::remove_all_cvrp_t<const volatile int&>, int>::value, "what?");

  const int* volatile* i[10];
  const auto& r = &i;
  static_assert(std::is_same<nstd::remove_all_cvrpe_t<decltype(i)>, int>::value, "what?");
  static_assert(std::is_same<nstd::remove_all_cvrpe_t<decltype(r)>, int>::value, "what?");

  static_assert(nstd::is_same_signed_v<unsigned int, unsigned short>, "what?");
  static_assert(nstd::is_same_signed_v<signed int, signed short>, "what?");
  static_assert(!nstd::is_same_signed_v<unsigned int, signed short>, "what?");

  A a;
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(a)), A&&>::value, "what?");
  B b;
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(b)), const B&>::value, "what?");
  C c;
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(c)), C&&>::value, "what?");
  D d;
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(d)), const D&>::value, "what?");

  return 0;
}
