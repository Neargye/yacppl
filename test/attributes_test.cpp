// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <attributes.hpp>

#include <stdexcept>

struct empty_type {};

struct holder {
  ATTR_NO_UNIQUE_ADDRESS empty_type empty;
  int value = 0;
};

ATTR_MAYBE_UNUSED static int unused_value = 0;

ATTR_NORETURN void throw_now() {
  throw std::runtime_error{"done"};
}

ATTR_ALWAYS_INLINE int inline_value(int value) {
  return value + 1;
}

ATTR_DEPRECATED("use nodiscard_value instead") int deprecated_value() {
  return 1;
}

ATTR_NODISCARD int nodiscard_value() {
  return 42;
}

ATTR_NODISCARD_MSG("use the computed value") int nodiscard_msg_value() {
  return 43;
}

struct ATTR_TRIVIAL_ABI trivial_abi_type {
  int value = 0;
};

int fallthrough_value(int value) {
  switch (value) {
    case 1:
      ATTR_FALLTHROUGH
    default:
      return 2;
  }
}

TEST_CASE("attributes macros compile and preserve runtime behavior") {
  holder h{};
  h.value = nodiscard_value();
  trivial_abi_type trivial_abi{};

  ATTR_ASSUME(h.value == 42);

  CHECK(h.value == 42);
  CHECK(inline_value(41) == 42);
  CHECK(nodiscard_msg_value() == 43);
  CHECK(trivial_abi.value == 0);
  CHECK(fallthrough_value(1) == 2);
  CHECK(fallthrough_value(0) == 2);
  CHECK(ATTR_LIKELY(h.value == 42));
  CHECK_FALSE(ATTR_UNLIKELY(h.value != 42));
  CHECK_THROWS_AS(throw_now(), const std::runtime_error&);
}
