// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <attributes.hpp>

#include <exception>

ATTR_MAYBE_UNUSED float foo1(ATTR_MAYBE_UNUSED int i) {  // No warning: unused parameter 'i'.
  return 42.0f;
}

ATTR_NORETURN void foo2() { std::terminate(); }

ATTR_DEPRECATED("reason") void foo3() {}

ATTR_ALWAYS_INLINE int foo3_inline(int value) { return value + 1; }

ATTR_NODISCARD int foo4() { return 42; }

ATTR_NODISCARD_MSG("use the computed value") int foo5() { return 42; }

struct empty_type {};

struct ATTR_TRIVIAL_ABI trivial_abi_type {
  int value = 0;
};

struct foo6 {
  ATTR_NO_UNIQUE_ADDRESS empty_type empty;
  int value = 0;
};

int main() {
  ATTR_MAYBE_UNUSED int a = foo4();  // No warning: unused variable 'a'.
  ATTR_MAYBE_UNUSED int b = foo5();  // No warning: unused variable 'b'.
  ATTR_MAYBE_UNUSED foo6 c{};
  ATTR_MAYBE_UNUSED trivial_abi_type d{};

  ATTR_ASSUME(a == b);

  if (ATTR_LIKELY(a == b)) {
    c.value = foo3_inline(a);
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
