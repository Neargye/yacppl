// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <unused.hpp>

void foo_unused(int i) {
  NSTD_UNUSED(i); // No warning: NSTD_UNUSED parameter 'i'.
}

int main() {
  int a = 0;
  float b = 0.0f;
  char c[] = "test";
  NSTD_UNUSED(a, b, c); // No warning: NSTD_UNUSED variable 'a', 'b', 'c'.

  int d = 0;
  NSTD_UNUSED(d); // No warning: NSTD_UNUSED variable 'd'.

  int e = 0;
  float f = 0.0f;
  nstd::unused(e, f); // No warning: nstd::unused variable 'e', 'f'.

  return 0;
}
