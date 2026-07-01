// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <utility.hpp>

#include <cstdint>
#include <tuple>
#include <type_traits>

enum class color : std::uint8_t {
  red = 1,
  green = 2,
  blue = 3
};

struct bytes {
  unsigned char value[sizeof(std::uint32_t)];
};

int main() {
  int sum = 0;
  nstd::invoke_each([&sum](int value) {
    sum += value;
  }, 1, 2, 3);
  if (sum != 6) {
    return 1;
  }

  nstd::apply_each([&sum](int value) {
    sum += value;
  }, std::make_tuple(4, 5));
  if (sum != 15) {
    return 1;
  }

  const std::uint32_t value = 0x12345678u;
  const auto raw = nstd::bit_cast<bytes>(value);
  const auto roundtrip = nstd::bit_cast<std::uint32_t>(raw);
  if (roundtrip != value) {
    return 1;
  }

  static_assert(nstd::to_underlying(color::green) == 2, "");
  static_assert(nstd::cmp_less(-1, 0u), "");
  static_assert(nstd::in_range<std::uint8_t>(255), "");

  int object = 42;
  static_assert(std::is_same<decltype(nstd::forward_like<const int&>(object)), const int&>::value, "");

  return 0;
}
