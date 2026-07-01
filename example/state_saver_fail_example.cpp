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

#include <state_saver.hpp>

#include <iostream>
#include <stdexcept>

void foo1(int& a) {
  SAVER_FAIL(a); // Restore during exception unwinding.

  a = 1;
  std::cout << "foo1 a = " << a << std::endl;
  throw std::runtime_error{"error"};
  // The original state is restored automatically during exception unwinding.
}

void foo2(int& a) {
  SAVER_FAIL(a); // Restore only during exception unwinding.

  a = 2;
  std::cout << "foo2 a = " << a << std::endl;
  // The original state is not restored when the scope exits without a new exception.
}

void foo3(int& a) {
#if defined(__cpp_deduction_guides) && __cpp_deduction_guides >= 201611L
  nstd::saver_fail state_saver{a}; // Named failure saver, without macros.
#else
  nstd::saver_fail<decltype(a)> state_saver{a}; // Named failure saver, without macros.
#endif
  a = 3;
  std::cout << "foo3 a = " << a << std::endl;
  throw std::runtime_error{"error"};
  // The original state is restored automatically during exception unwinding.
}

void foo4(int& a) {
  MAKE_SAVER_FAIL(state_saver, a); // Named failure saver.

  a = 4;
  std::cout << "foo4 a = " << a << std::endl;

  state_saver.dismiss(); // Disable automatic restore.
  std::cout << "foo4 state_saver::dismiss" << std::endl;
  throw std::runtime_error{"error"};
  // The original state is not restored during exception unwinding.
}

void foo5(int& a) {
  MAKE_SAVER_FAIL(state_saver, a); // Named failure saver.

  a = 5;
  std::cout << "foo5 a = " << a << std::endl;

  state_saver.dismiss(); // Disable automatic restore.
  std::cout << "foo5 state_saver::dismiss" << std::endl;

  state_saver.restore(); // Restore immediately.
  std::cout << "foo5 state_saver::restore" << std::endl;
  std::cout << "foo5 a = " << a << std::endl;
  throw std::runtime_error{"error"};
  // The original state is not restored during exception unwinding.
}

void foo6(int& a) {
  WITH_SAVER_FAIL(a) {
    a = 1;
    std::cout << "foo6 a = " << a << std::endl;
    throw std::runtime_error{"error"};
    // The original state is restored automatically during exception unwinding.
  }

  std::cout << "foo6 a = " << a << std::endl;
}

int main() {
  int a = 0;
  std::cout << "main a = " << a << std::endl;

  try {
    foo1(a);
  } catch (...) {}
  std::cout << "main a = " << a << std::endl;

  foo2(a);
  std::cout << "main a = " << a << std::endl;

  try {
    foo3(a);
  } catch (...) {}
  std::cout << "main a = " << a << std::endl;

  try {
    foo4(a);
  } catch (...) {}
  std::cout << "main a = " << a << std::endl;

  try {
    foo5(a);
  } catch (...) {}
  std::cout << "main a = " << a << std::endl;

  try {
    foo6(a);
  } catch (...) {}
  std::cout << "main a = " << a << std::endl;

  return 0;
}
