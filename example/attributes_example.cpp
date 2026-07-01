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

#include <attributes.hpp>

#include <exception>

ATTR_MAYBE_UNUSED float foo1(ATTR_MAYBE_UNUSED int i) {  // No warning: unused parameter 'i'.
  return 42.0f;
}

ATTR_NORETURN void foo2() { std::terminate(); }

ATTR_DEPRECATED("reason") void foo3() {}

ATTR_NODISCARD int foo4() { return 42; }

ATTR_NODISCARD_MSG("use the computed value") int foo5() { return 42; }

struct empty_type {};

struct foo6 {
  ATTR_NO_UNIQUE_ADDRESS empty_type empty;
  int value = 0;
};

int main() {
  ATTR_MAYBE_UNUSED int a = foo4();  // No warning: unused variable 'a'.
  ATTR_MAYBE_UNUSED int b = foo5();  // No warning: unused variable 'b'.
  ATTR_MAYBE_UNUSED foo6 c{};

  if (ATTR_LIKELY(a == b)) {
    c.value = a;
  } else if (ATTR_UNLIKELY(a == 0)) {
    return 1;
  }

  switch (a) {
    case 1:
      ATTR_FALLTHROUGH
    default:
      break;
  }

  return 0;
}
