// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2021 Daniil Goncharov <neargye@gmail.com>.
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
  SAVER_SUCCESS(a); // State saver on success.

  a = 1;
  std::cout << "foo1 a = " << a << std::endl;
 // Original state will automatically restored, on scope leave when no exceptions have been thrown.
}

void foo2(int& a) {
  SAVER_SUCCESS(a); // Custom state saver on success.

  a = 2;
  std::cout << "foo2 a = " << a << std::endl;
  throw std::runtime_error{"error"};
  // Original state will not automatically restored, on error.
}

void foo3(int& a) {
#if defined(__cpp_deduction_guides) && __cpp_deduction_guides >= 201611L
  nstd::saver_success state_saver{a}; // Custom state saver on success, without macros.
#else
  nstd::saver_success<decltype(a)> state_saver{a}; // Custom state saver on success, without macros.
#endif
  a = 3;
  std::cout << "foo3 a = " << a << std::endl;
  // Original state will automatically restored, on scope leave when no exceptions have been thrown.
}

void foo4(int& a) {
  MAKE_SAVER_EXIT(state_saver, a); // Custom state saver on success.

  a = 4;
  std::cout << "foo4 a = " << a << std::endl;

  state_saver.dismiss(); // Dismiss, state will not automatically restored.
  std::cout << "foo4 state_saver::dismiss" << std::endl;
  // Original state will not automatically restored, on scope leave when no exceptions have been thrown.
}

void foo5(int& a) {
  MAKE_SAVER_EXIT(state_saver, a); // Custom state saver on success.

  a = 5;
  std::cout << "foo5 a = " << a << std::endl;

  state_saver.dismiss(); // Dismiss, state will not automatically restored.
  std::cout << "foo5 state_saver::dismiss" << std::endl;

  state_saver.restore(); // Restore state.
  std::cout << "foo5 state_saver::restore" << std::endl;
  std::cout << "foo5 a = " << a << std::endl;
  // Original state will not automatically restored, on scope leave when no exceptions have been thrown.
}

void foo6(int& a) {
  WITH_SAVER_SUCCESS(a) {
    a = 1;
    std::cout << "foo6 a = " << a << std::endl;
    // Original state will automatically restored, on scope leave when no exceptions have been thrown.
  }

  std::cout << "foo6 a = " << a << std::endl;
}

int main() {
  int a = 0;
  std::cout << "main a = " << a << std::endl;

  foo1(a);
  std::cout << "main a = " << a << std::endl;

  try {
    foo2(a);
  } catch (...) {}
  std::cout << "main a = " << a << std::endl;

  foo3(a);
  std::cout << "main a = " << a << std::endl;

  foo4(a);
  std::cout << "main a = " << a << std::endl;

  foo5(a);
  std::cout << "main a = " << a << std::endl;

  foo6(a);
  std::cout << "main a = " << a << std::endl;

  return 0;
}
