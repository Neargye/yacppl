// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

// doctest requires C++11, so this C++98 test reports failures through the exit code.

#include <attributes.hpp>

static int fallthrough_value(int value) {
  switch (value) {
    case 1:
      ++value;
      ATTR_FALLTHROUGH
    case 2:
      return value;
    default:
      return 0;
  }
}

int main() {
  return fallthrough_value(1) == 2 && fallthrough_value(2) == 2 && fallthrough_value(3) == 0 ? 0 : 1;
}
