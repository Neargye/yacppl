// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <utility.hpp>

#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace {

struct copy_only_assignable {
  copy_only_assignable() = default;
  copy_only_assignable(copy_only_assignable&&) noexcept = default;
  copy_only_assignable(const copy_only_assignable&) = default;
  copy_only_assignable& operator=(const copy_only_assignable&) = default;
  copy_only_assignable& operator=(copy_only_assignable&&) = delete;
};

struct throwing_move_assignable {
  throwing_move_assignable() = default;
  throwing_move_assignable(throwing_move_assignable&&) noexcept = default;
  throwing_move_assignable(const throwing_move_assignable&) = default;
  throwing_move_assignable& operator=(const throwing_move_assignable&) = default;
  throwing_move_assignable& operator=(throwing_move_assignable&&) noexcept(false) {
    return *this;
  }
};

struct byte_array {
  unsigned char value[sizeof(std::uint32_t)];
};

struct lvalue_only_callable {
  int* sum;

  void operator()(int value) & noexcept {
    *sum += value;
  }

  void operator()(int) && = delete;
};

enum class small_enum : unsigned char {
  value = 7
};

} // namespace

TEST_CASE("move helpers preserve intended reference categories") {
  int value = 1;
  copy_only_assignable copy_only;
  throwing_move_assignable throwing_move;

  static_assert(std::is_same<decltype(nstd::move(value)), int&&>::value, "");
  static_assert(std::is_same<decltype(nstd::move_if_noexcept(value)), int&&>::value, "");
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(copy_only)), const copy_only_assignable&>::value, "");
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(throwing_move)), const throwing_move_assignable&>::value, "");

  CHECK(nstd::unforward(value) == 1);
  CHECK(nstd::decay_copy(value) == 1);
}

TEST_CASE("bit_cast round-trips trivially copyable values") {
  const std::uint32_t value = 0x12345678u;

  const auto bytes = nstd::bit_cast<byte_array>(value);
  const auto roundtrip = nstd::bit_cast<std::uint32_t>(bytes);

  CHECK(roundtrip == value);
}

TEST_CASE("invoke_each invokes the same callable as an lvalue") {
  int sum = 0;

  nstd::invoke_each([](int) {});
  nstd::invoke_each(lvalue_only_callable{&sum}, 1, 2, 3);

  CHECK(sum == 6);
}

TEST_CASE("apply_each supports tuple-like lvalues, const lvalues, and rvalues") {
  int sum = 0;
  auto empty = std::make_tuple();
  auto values = std::make_tuple(1, 2, 3);
  const auto const_values = std::make_pair(4, 5);
  std::array<int, 2> array_values{{6, 7}};

  nstd::apply_each([](int) {}, empty);
  nstd::apply_each([&sum](int value) { sum += value; }, values);
  nstd::apply_each([&sum](int value) { sum += value; }, const_values);
  nstd::apply_each([&sum](int value) { sum += value; }, array_values);
  nstd::apply_each([&sum](std::unique_ptr<int> value) { sum += *value; },
                   std::make_tuple(std::unique_ptr<int>{new int{8}}));

  CHECK(sum == 36);
}

TEST_CASE("forward_like applies cv-ref qualifiers from the model type") {
  int value = 42;

  static_assert(std::is_same<decltype(nstd::forward_like<int&>(value)), int&>::value, "");
  static_assert(std::is_same<decltype(nstd::forward_like<const int&>(value)), const int&>::value, "");
  static_assert(std::is_same<decltype(nstd::forward_like<volatile int&&>(value)), volatile int&&>::value, "");
  static_assert(std::is_same<decltype(nstd::forward_like<const volatile int>(value)), const volatile int&&>::value, "");
}

TEST_CASE("to_underlying converts enum values to their underlying type") {
  static_assert(std::is_same<decltype(nstd::to_underlying(small_enum::value)), unsigned char>::value, "");

  CHECK(nstd::to_underlying(small_enum::value) == 7);
}

TEST_CASE("integer comparison helpers compare signed and unsigned values safely") {
  static_assert(nstd::cmp_equal(42, 42u), "");
  static_assert(!nstd::cmp_equal(-1, std::numeric_limits<unsigned>::max()), "");
  static_assert(nstd::cmp_not_equal(-1, std::numeric_limits<unsigned>::max()), "");
  static_assert(nstd::cmp_less(-1, 0u), "");
  static_assert(nstd::cmp_less(0u, 1), "");
  static_assert(nstd::cmp_greater(1u, 0), "");
  static_assert(nstd::cmp_less_equal(0, 0u), "");
  static_assert(nstd::cmp_greater_equal(0u, 0), "");

  CHECK(nstd::cmp_equal(42, 42u));
  CHECK_FALSE(nstd::cmp_equal(-1, std::numeric_limits<unsigned>::max()));
  CHECK(nstd::cmp_less(-1, 0u));
  CHECK_FALSE(nstd::cmp_less(std::numeric_limits<unsigned>::max(), -1));
}

TEST_CASE("in_range checks whether integer values fit in the target type") {
  static_assert(nstd::in_range<unsigned char>(0), "");
  static_assert(nstd::in_range<unsigned char>(255), "");
  static_assert(!nstd::in_range<unsigned char>(-1), "");
  static_assert(!nstd::in_range<signed char>(128u), "");
  static_assert(nstd::in_range<int>(42u), "");

  CHECK(nstd::in_range<unsigned char>(255));
  CHECK_FALSE(nstd::in_range<unsigned char>(-1));
  CHECK_FALSE(nstd::in_range<signed char>(128u));
}

TEST_CASE("constexpr_for iterates over the requested range") {
  constexpr int sum = [] {
    int value = 0;
    nstd::constexpr_for<0, 5, 1>([&value](auto index) {
      value += index;
    });
    return value;
  }();

  static_assert(sum == 10, "");
  CHECK(sum == 10);
}
