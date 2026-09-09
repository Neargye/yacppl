// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Daniil Goncharov <neargye@gmail.com>.

#ifndef YACPPL_REPRESENTATION_TEST_TYPES_HPP
#define YACPPL_REPRESENTATION_TEST_TYPES_HPP

#include <cstdint>
#include <type_traits>

namespace representation_test {

struct address_trap {
  std::uint32_t value;
  inline static std::uint32_t unrelated = 99;

  std::uint32_t* operator&() noexcept {
    return &unrelated;
  }

  const std::uint32_t* operator&() const noexcept {
    return &unrelated;
  }
};

struct deleted_address {
  std::uint32_t value;

  deleted_address* operator&() = delete;
  const deleted_address* operator&() const = delete;
};

struct copy_only {
  std::uint32_t value;

  copy_only() = default;
  copy_only(const copy_only&) = default;
  copy_only(copy_only&&) = delete;
};

struct move_only {
  std::uint32_t value;

  move_only() = default;
  move_only(const move_only&) = delete;
  move_only(move_only&&) = default;
};

struct explicit_copy {
  std::uint32_t value;

  explicit_copy() = default;
  explicit explicit_copy(const explicit_copy&) = default;

  template <typename T>
  explicit_copy(T&& other) noexcept : value(other.value + 1) {}
};

struct unsafe_return {
  std::uint32_t value;

  unsafe_return() = default;
  unsafe_return(const unsafe_return&) = delete;
  unsafe_return& operator=(const unsafe_return&) = default;

  template <typename T>
  unsafe_return(T&& other) noexcept : value(other.value + 1) {}
};

static_assert(std::is_trivially_copyable_v<unsafe_return>);
static_assert(std::is_convertible_v<unsafe_return&&, unsafe_return>);
static_assert(!std::is_trivially_move_constructible_v<unsafe_return>);
static_assert(std::is_trivially_copy_constructible_v<explicit_copy>);
static_assert(!std::is_trivially_move_constructible_v<explicit_copy>);

} // namespace representation_test

#endif // YACPPL_REPRESENTATION_TEST_TYPES_HPP
