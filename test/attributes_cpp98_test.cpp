// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

// attributes.hpp supports C++98, where doctest is not available.
#include <attributes.hpp>

// Must not trigger -Wimplicit-fallthrough.
int fallthrough_value(int value) {
  int result = 0;
  switch (value) {
    case 1:
      result += 1;
      ATTR_FALLTHROUGH
    case 2:
      result += 2;
      break;
    default:
      break;
  }
  return result;
}

int main() {
  return (fallthrough_value(1) == 3 && fallthrough_value(2) == 2 && fallthrough_value(0) == 0) ? 0 : 1;
}
