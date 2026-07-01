// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <unused.hpp>

namespace {

int side_effect_counter = 0;

int increment_counter() {
  return ++side_effect_counter;
}

struct move_only {
  move_only() = default;
  move_only(const move_only&) = delete;
  move_only& operator=(const move_only&) = delete;
};

struct bit_field_holder {
  unsigned value : 3;
};

} // namespace

TEST_CASE("NSTD_UNUSED supports variables and parameters") {
  side_effect_counter = 0;
  int value = 1;
  const int const_value = 2;
  volatile int volatile_value = 3;
  move_only object;
  bit_field_holder bits{4};

  NSTD_UNUSED(value, const_value, volatile_value, object, bits.value);

  CHECK(side_effect_counter == 0);
  CHECK(value == 1);
  CHECK(const_value == 2);
  CHECK(volatile_value == 3);
  CHECK(bits.value == 4);
}

TEST_CASE("NSTD_UNUSED does not evaluate arguments on non-MSVC compilers") {
  side_effect_counter = 0;

#if !defined(_MSC_VER)
  NSTD_UNUSED(increment_counter());

  CHECK(side_effect_counter == 0);
#else
  NSTD_UNUSED(side_effect_counter);

  CHECK(side_effect_counter == 0);
#endif
}

TEST_CASE("nstd::unused is a regular function call") {
  side_effect_counter = 0;

  nstd::unused(increment_counter());

  CHECK(side_effect_counter == 1);
}

TEST_CASE("nstd::unused accepts multiple non-copying arguments") {
  int value = 1;
  const int const_value = 2;
  volatile int volatile_value = 3;
  move_only object;
  bit_field_holder bits{4};

  nstd::unused(value, const_value, volatile_value, object, bits.value);

  CHECK(value == 1);
  CHECK(const_value == 2);
  CHECK(volatile_value == 3);
  CHECK(bits.value == 4);
}
