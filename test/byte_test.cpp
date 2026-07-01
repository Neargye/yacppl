// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <byte.hpp>

#include <array>
#include <cstddef>
#include <type_traits>

namespace {

struct payload {
  int value;
  unsigned short flags;
};

struct non_default_payload {
  int value;

  non_default_payload() = delete;
  explicit non_default_payload(int value) : value{value} {}
};

static_assert(std::is_trivially_copyable<nstd::byte>::value, "nstd::byte must be trivially copyable.");
static_assert(sizeof(nstd::byte) == sizeof(unsigned char), "nstd::byte must have byte-sized storage.");
static_assert(std::is_trivially_copyable<non_default_payload>::value, "non_default_payload must be trivially copyable.");
static_assert(nstd::to_integer(nstd::to_byte(0x7f)) == 0x7f, "integer conversion must round-trip byte values.");
static_assert(nstd::to_integer(nstd::to_byte(0x0f) | nstd::to_byte(0xf0)) == 0xff, "byte bitwise or must work.");
static_assert(nstd::to_integer(nstd::to_byte(0xf0) & nstd::to_byte(0x0f)) == 0x00, "byte bitwise and must work.");
static_assert(nstd::to_integer(nstd::to_byte(0xaa) ^ nstd::to_byte(0xff)) == 0x55, "byte bitwise xor must work.");
static_assert(nstd::to_integer(~nstd::to_byte(0x0f)) == 0xf0, "byte bitwise not must work.");
static_assert(nstd::to_integer(nstd::to_byte(0x01) << 3) == 0x08, "byte shift left must work on rvalues.");
static_assert(nstd::to_integer(nstd::to_byte(0x80) >> 7) == 0x01, "byte shift right must work on rvalues.");

} // namespace

TEST_CASE("byte bitwise operations match unsigned byte semantics") {
  nstd::byte value = nstd::to_byte(0x01);

  value <<= 3;
  CHECK(nstd::to_integer(value) == 0x08);

  value |= nstd::to_byte(0x80);
  CHECK(nstd::to_integer(value) == 0x88);

  value &= nstd::to_byte(0x8f);
  CHECK(nstd::to_integer(value) == 0x88);

  value ^= nstd::to_byte(0x08);
  CHECK(nstd::to_integer(value) == 0x80);

  value >>= 7;
  CHECK(nstd::to_integer(value) == 0x01);
}

TEST_CASE("byte conversion helpers preserve trivially copyable objects") {
  const payload original{42, 7};
  std::array<nstd::byte, sizeof(payload)> bytes{};

  static_assert(noexcept(nstd::to_bytes(bytes.data(), original)), "to_bytes must be noexcept for single objects.");
  static_assert(noexcept(nstd::from_bytes<payload>(bytes.data())), "from_bytes<T> must be noexcept for nothrow default constructible objects.");

  nstd::to_bytes(bytes.data(), original);
  const auto restored = nstd::from_bytes<payload>(bytes.data());

  CHECK(restored.value == original.value);
  CHECK(restored.flags == original.flags);
}

TEST_CASE("byte conversion helpers can restore into an existing object") {
  const non_default_payload original{42};
  non_default_payload restored{0};
  std::array<nstd::byte, sizeof(non_default_payload)> bytes{};

  static_assert(noexcept(nstd::from_bytes(restored, bytes.data())), "from_bytes reference overload must be noexcept.");

  nstd::to_bytes(bytes.data(), original);
  nstd::from_bytes(restored, bytes.data());

  CHECK(restored.value == original.value);
}

TEST_CASE("byte conversion helpers preserve arrays of trivially copyable objects") {
  const payload original[] = {{1, 2}, {3, 4}, {5, 6}};
  payload restored[] = {{0, 0}, {0, 0}, {0, 0}};
  std::array<nstd::byte, sizeof(original)> bytes{};

  static_assert(noexcept(nstd::to_bytes(bytes.data(), original, 3)), "to_bytes must be noexcept for arrays.");
  static_assert(noexcept(nstd::from_bytes(restored, bytes.data(), 3)), "from_bytes must be noexcept for arrays.");
  static_assert(noexcept(nstd::to_bytes(bytes.data(), original)), "to_bytes must be noexcept for bounded arrays.");
  static_assert(noexcept(nstd::from_bytes(restored, bytes.data())), "from_bytes must be noexcept for bounded arrays.");

  nstd::to_bytes(bytes.data(), original);
  nstd::from_bytes(restored, bytes.data());

  for (std::size_t i = 0; i < 3; ++i) {
    CHECK(restored[i].value == original[i].value);
    CHECK(restored[i].flags == original[i].flags);
  }
}
