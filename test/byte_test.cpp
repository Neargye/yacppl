// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <byte.hpp>

#include "representation_test_types.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace byte_adl_test {

struct payload {
  int value;
};

void to_bytes(nstd::byte*, const payload*, std::size_t) = delete;
void from_bytes(payload*, const nstd::byte*, std::size_t) = delete;

} // namespace byte_adl_test

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

struct nontrivial_payload {
  std::uint32_t value;

  nontrivial_payload() = default;
  nontrivial_payload(const nontrivial_payload& other) : value{other.value} {}
};

struct nonreturnable_payload {
  unsigned int value;

  nonreturnable_payload() = default;
  nonreturnable_payload(const nonreturnable_payload&) = delete;
  nonreturnable_payload(nonreturnable_payload&&) = delete;
  nonreturnable_payload& operator=(const nonreturnable_payload&) = default;
};

struct alignas(8) padded_payload {
  unsigned char tag;
  std::uint32_t value;
};

struct nontrivial_default_payload {
  std::uint32_t value;

  nontrivial_default_payload() noexcept : value{} {}
};

struct potentially_throwing_default_payload {
  std::uint32_t value;

  potentially_throwing_default_payload() noexcept(false) : value{} {}
};

struct potentially_throwing_return_payload {
  std::uint32_t value;

  potentially_throwing_return_payload() noexcept = default;
  potentially_throwing_return_payload(const potentially_throwing_return_payload&) = default;

  template <typename U, typename std::enable_if<std::is_same<typename std::remove_reference<U>::type, potentially_throwing_return_payload>::value && !std::is_lvalue_reference<U>::value, int>::type = 0>
  potentially_throwing_return_payload(U&& other) noexcept(false) : value{other.value + 1} {
    throw 1;
  }
};

struct explicit_only_return_payload {
  std::uint32_t value;

  explicit_only_return_payload() = default;
  explicit_only_return_payload(const explicit_only_return_payload&) = delete;

  template <typename U, typename std::enable_if<std::is_same<typename std::remove_reference<U>::type, explicit_only_return_payload>::value, int>::type = 0>
  explicit explicit_only_return_payload(U&& other) noexcept : value{other.value} {}
};

struct explicit_throwing_return_payload {
  std::uint32_t value;

  explicit_throwing_return_payload() = default;
  explicit_throwing_return_payload(const explicit_throwing_return_payload&) = default;

  template <typename U, typename std::enable_if<std::is_same<typename std::remove_reference<U>::type, explicit_throwing_return_payload>::value && !std::is_lvalue_reference<U>::value, int>::type = 0>
  explicit explicit_throwing_return_payload(U&& other) noexcept(false) : value{other.value} {}
};

enum class payload_kind : std::uint16_t {
  data = 7
};

template <typename T, typename = void>
struct can_shift_byte_left : std::false_type {};

template <typename T>
struct can_shift_byte_left<T, std::void_t<decltype(nstd::to_byte(1) << std::declval<T>())>> : std::true_type {};

template <typename T, typename = void>
struct can_from_bytes : std::false_type {};

template <typename T>
struct can_from_bytes<T, std::void_t<decltype(nstd::from_bytes<T>(std::declval<const nstd::byte*>()))>> : std::true_type {};

template <typename T, typename = void>
struct can_to_bytes : std::false_type {};

template <typename T>
struct can_to_bytes<T, std::void_t<decltype(nstd::to_bytes(std::declval<nstd::byte*>(), std::declval<const T&>()))>> : std::true_type {};

template <typename T, typename = void>
struct can_from_bytes_into : std::false_type {};

template <typename T>
struct can_from_bytes_into<T, std::void_t<decltype(nstd::from_bytes(std::declval<T&>(), std::declval<const nstd::byte*>()))>> : std::true_type {};

template <typename T>
T round_trip(const T& original) {
  std::array<nstd::byte, sizeof(T)> bytes{};
  nstd::to_bytes(bytes.data(), original);
  return nstd::from_bytes<T>(bytes.data());
}

constexpr auto expected_byte_not = static_cast<unsigned char>(~static_cast<unsigned int>(0x0f));
constexpr auto expected_byte_shift = static_cast<unsigned char>(static_cast<unsigned int>(0x01) << 8);

static_assert(std::is_trivially_copyable<nstd::byte>::value, "nstd::byte must be trivially copyable.");
static_assert(sizeof(nstd::byte) == sizeof(unsigned char), "nstd::byte must have byte-sized storage.");
static_assert(std::is_trivially_copyable<non_default_payload>::value, "non_default_payload must be trivially copyable.");
static_assert(!std::is_trivially_copyable<nontrivial_payload>::value, "nontrivial_payload must not be trivially copyable.");
static_assert(!can_to_bytes<nontrivial_payload>::value, "to_bytes must reject non-trivially-copyable sources.");
static_assert(!can_from_bytes<non_default_payload>::value, "from_bytes<T> must reject types without a default constructor.");
static_assert(!can_from_bytes_into<nontrivial_payload>::value, "from_bytes(T&) must reject non-trivially-copyable destinations.");
static_assert(std::is_trivially_copyable<nonreturnable_payload>::value, "nonreturnable_payload must be trivially copyable.");
static_assert(std::is_trivially_default_constructible<nonreturnable_payload>::value, "nonreturnable_payload must be trivially default constructible.");
static_assert(!std::is_constructible<nonreturnable_payload, nonreturnable_payload&&>::value, "nonreturnable_payload must not be returnable by value.");
static_assert(!can_from_bytes<nonreturnable_payload>::value, "from_bytes<T> must reject types that cannot be returned by value.");
static_assert(std::is_trivially_copyable<padded_payload>::value, "padded_payload must be trivially copyable.");
static_assert(sizeof(padded_payload) > sizeof(unsigned char) + sizeof(std::uint32_t), "padded_payload must contain padding on this target.");
static_assert(std::is_trivially_copyable<nontrivial_default_payload>::value, "nontrivial_default_payload must be trivially copyable.");
static_assert(!std::is_trivially_default_constructible<nontrivial_default_payload>::value, "nontrivial_default_payload must have a non-trivial default constructor.");
static_assert(can_from_bytes<nontrivial_default_payload>::value, "from_bytes<T> should accept a non-trivial default constructor.");
static_assert(std::is_trivially_copyable<potentially_throwing_default_payload>::value, "potentially_throwing_default_payload must be trivially copyable.");
static_assert(can_from_bytes<potentially_throwing_default_payload>::value, "from_bytes<T> should accept a potentially throwing default constructor.");
static_assert(!noexcept(nstd::from_bytes<potentially_throwing_default_payload>(std::declval<const nstd::byte*>())), "from_bytes<T> must reflect a potentially throwing default constructor.");
static_assert(std::is_trivially_copyable<potentially_throwing_return_payload>::value, "potentially_throwing_return_payload must be trivially copyable.");
static_assert(can_from_bytes<potentially_throwing_return_payload>::value, "from_bytes<T> should accept a returnable type.");
static_assert(noexcept(nstd::from_bytes<potentially_throwing_return_payload>(std::declval<const nstd::byte*>())), "from_bytes<T> must use the trivial copy constructor.");
static_assert(std::is_trivially_copyable<explicit_only_return_payload>::value, "explicit_only_return_payload must be trivially copyable.");
static_assert(std::is_constructible<explicit_only_return_payload, explicit_only_return_payload&&>::value, "explicit_only_return_payload must expose the constructible/convertible distinction.");
static_assert(!std::is_convertible<explicit_only_return_payload&&, explicit_only_return_payload>::value, "explicit_only_return_payload must not support implicit return construction.");
static_assert(!can_from_bytes<explicit_only_return_payload>::value, "from_bytes<T> must reject a type that only has an explicit return constructor.");
static_assert(std::is_trivially_copyable<explicit_throwing_return_payload>::value, "explicit_throwing_return_payload must be trivially copyable.");
static_assert(!std::is_nothrow_constructible<explicit_throwing_return_payload, explicit_throwing_return_payload&&>::value, "direct construction must select the explicit throwing constructor.");
static_assert(std::is_convertible<explicit_throwing_return_payload&&, explicit_throwing_return_payload>::value, "implicit return construction must remain available through the copy constructor.");
static_assert(can_from_bytes<explicit_throwing_return_payload>::value, "from_bytes<T> should accept an implicitly returnable type.");
static_assert(noexcept(nstd::from_bytes<explicit_throwing_return_payload>(std::declval<const nstd::byte*>())), "from_bytes<T> must avoid the non-trivial rvalue constructor.");
static_assert(!can_to_bytes<volatile std::uint32_t>::value, "to_bytes must reject volatile sources that memcpy cannot access.");
static_assert(!can_from_bytes<const std::uint32_t>::value, "from_bytes<T> must reject const destinations.");
static_assert(!can_from_bytes<volatile std::uint32_t>::value, "from_bytes<T> must reject volatile destinations.");
static_assert(!can_from_bytes_into<const std::uint32_t>::value, "from_bytes(T&) must reject const destinations.");
static_assert(!can_from_bytes_into<volatile std::uint32_t>::value, "from_bytes(T&) must reject volatile destinations.");
static_assert(can_shift_byte_left<int>::value, "byte shift must accept integral counts.");
static_assert(can_shift_byte_left<unsigned int>::value, "byte shift must accept unsigned integral counts.");
static_assert(!can_shift_byte_left<bool>::value, "byte shift must reject bool counts.");
static_assert(nstd::detail::is_valid_byte_shift(0), "zero shift must be accepted.");
static_assert(!nstd::detail::is_valid_byte_shift(-1), "negative shifts must be rejected.");
static_assert(nstd::detail::is_valid_byte_shift(std::numeric_limits<unsigned int>::digits - 1), "the highest valid signed shift must be accepted.");
static_assert(!nstd::detail::is_valid_byte_shift(std::numeric_limits<unsigned int>::digits), "a shift by the bit width must be rejected.");
static_assert(nstd::detail::is_valid_byte_shift(static_cast<unsigned int>(std::numeric_limits<unsigned int>::digits - 1)), "the highest valid unsigned shift must be accepted.");
static_assert(!nstd::detail::is_valid_byte_shift(static_cast<unsigned int>(std::numeric_limits<unsigned int>::digits)), "an unsigned shift by the bit width must be rejected.");
static_assert(!nstd::detail::is_valid_byte_shift((std::numeric_limits<unsigned long long>::max)()), "wide shift counts must not be truncated before validation.");
static_assert(nstd::to_integer(nstd::to_byte(0x7f)) == 0x7f, "integer conversion must round-trip byte values.");
static_assert(nstd::to_integer(nstd::to_byte(0x0f) | nstd::to_byte(0xf0)) == 0xff, "byte bitwise or must work.");
static_assert(nstd::to_integer(nstd::to_byte(0xf0) & nstd::to_byte(0x0f)) == 0x00, "byte bitwise and must work.");
static_assert(nstd::to_integer(nstd::to_byte(0xaa) ^ nstd::to_byte(0xff)) == 0x55, "byte bitwise xor must work.");
static_assert(nstd::to_integer(~nstd::to_byte(0x0f)) == expected_byte_not, "byte bitwise not must work.");
static_assert(nstd::to_integer(nstd::to_byte(0x01) << 3) == 0x08, "byte shift left must work on rvalues.");
static_assert(nstd::to_integer(nstd::to_byte(0x01) << 3u) == 0x08, "byte shift left must accept unsigned counts.");
static_assert(nstd::to_integer(nstd::to_byte(0x01) << 8) == expected_byte_shift, "byte shift must follow unsigned int shift semantics.");
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

TEST_CASE("byte copies bypass overloaded addresses and non-trivial return constructors") {
  using namespace representation_test;
  static_assert(!can_from_bytes<unsafe_return>::value, "a non-trivial return constructor must be rejected.");

  const address_trap original{7};
  address_trap::unrelated = 99;
  std::array<nstd::byte, sizeof(original)> bytes{};
  nstd::to_bytes(bytes.data(), original);
  CHECK(nstd::from_bytes<std::uint32_t>(bytes.data()) == 7);
  CHECK(nstd::from_bytes<address_trap>(bytes.data()).value == 7);
  address_trap destination{0};
  nstd::from_bytes(destination, bytes.data());
  CHECK(destination.value == 7);
  CHECK(address_trap::unrelated == 99);

  const deleted_address source{7};
  deleted_address target{0};
  nstd::to_bytes(bytes.data(), source);
  nstd::from_bytes(target, bytes.data());
  CHECK(target.value == 7);
  CHECK(nstd::from_bytes<deleted_address>(bytes.data()).value == 7);
  CHECK(nstd::from_bytes<copy_only>(bytes.data()).value == 7);
  CHECK(nstd::from_bytes<move_only>(bytes.data()).value == 7);
  CHECK(nstd::from_bytes<explicit_copy>(bytes.data()).value == 7);

#if defined(NDEBUG)
  CHECK(nstd::from_bytes<copy_only>(nullptr).value == 0);
  CHECK(nstd::from_bytes<move_only>(nullptr).value == 0);
  CHECK(nstd::from_bytes<explicit_copy>(nullptr).value == 0);
#endif
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

TEST_CASE("bounded byte copies ignore unrelated ADL overloads") {
  const byte_adl_test::payload original[2]{{17}, {42}};
  byte_adl_test::payload restored[2]{};
  std::array<nstd::byte, sizeof(original)> bytes{};

  nstd::to_bytes(bytes.data(), original);
  nstd::from_bytes(restored, bytes.data());

  CHECK(restored[0].value == 17);
  CHECK(restored[1].value == 42);
}

TEST_CASE("byte conversion helpers round-trip trusted object representations") {
  const auto integer = round_trip<std::int64_t>(-123456789);
  const auto floating = round_trip<double>(-123.5);
  const auto kind = round_trip<payload_kind>(payload_kind::data);
  const auto padded = round_trip<padded_payload>(padded_payload{3, 42});
  const auto custom_default = round_trip<nontrivial_default_payload>(nontrivial_default_payload{});
  potentially_throwing_return_payload throwing_return_source;
  throwing_return_source.value = 88;
  const auto throwing_return = round_trip<potentially_throwing_return_payload>(throwing_return_source);
  explicit_throwing_return_payload explicit_return_source;
  explicit_return_source.value = 89;
  const auto explicit_return = round_trip<explicit_throwing_return_payload>(explicit_return_source);

  CHECK(integer == -123456789);
  CHECK(floating == -123.5);
  CHECK(kind == payload_kind::data);
  CHECK(padded.tag == 3);
  CHECK(padded.value == 42);
  CHECK(custom_default.value == 0);
  CHECK(throwing_return.value == 88);
  CHECK(explicit_return.value == 89);

  nonreturnable_payload original;
  original.value = 77;
  nonreturnable_payload restored;
  restored.value = 0;
  std::array<nstd::byte, sizeof(nonreturnable_payload)> bytes{};
  nstd::to_bytes(bytes.data(), original);
  nstd::from_bytes(restored, bytes.data());
  CHECK(restored.value == original.value);
}

TEST_CASE("counted byte copies define zero, overflow, and null behavior") {
  nstd::to_bytes<std::uint32_t>(nullptr, nullptr, 0);
  nstd::from_bytes<std::uint32_t>(nullptr, nullptr, 0);

  constexpr auto max_count = (std::numeric_limits<std::size_t>::max)() / sizeof(payload);
  constexpr auto overflow_count = max_count + 1;
  static_assert(nstd::detail::is_valid_byte_count<payload>(max_count), "maximum non-overflowing count must be accepted.");
  static_assert(!nstd::detail::is_valid_byte_count<payload>(overflow_count), "overflowing count must be rejected.");

#if defined(NDEBUG)
  const payload source{42, 7};
  payload destination{1, 2};
  std::array<nstd::byte, sizeof(payload)> bytes{};
  bytes.fill(nstd::to_byte(0xa5));

  nstd::to_bytes(bytes.data(), &source, overflow_count);
  for (const auto value : bytes) {
    CHECK(value == nstd::to_byte(0xa5));
  }

  nstd::from_bytes(&destination, bytes.data(), overflow_count);
  CHECK(destination.value == 1);
  CHECK(destination.flags == 2);

  const std::uint32_t scalar_source = 42;
  std::uint32_t scalar_destination = 1;
  nstd::to_bytes<std::uint32_t>(nullptr, scalar_source);
  nstd::from_bytes<std::uint32_t>(scalar_destination, nullptr);
  CHECK(scalar_destination == 1);
  CHECK(nstd::from_bytes<std::uint32_t>(nullptr) == 0);
  nstd::to_bytes<std::uint32_t>(nullptr, &scalar_source, 1);
  nstd::to_bytes<std::uint32_t>(bytes.data(), nullptr, 1);
  nstd::from_bytes<std::uint32_t>(nullptr, bytes.data(), 1);
  nstd::from_bytes<std::uint32_t>(&scalar_destination, nullptr, 1);
  CHECK(scalar_destination == 1);
#endif
}

TEST_CASE("byte conversion helpers support overlapping ranges") {
  std::array<nstd::byte, 8> bytes{};
  for (std::size_t i = 0; i < bytes.size(); ++i) {
    bytes[i] = nstd::to_byte(i);
  }

  nstd::to_bytes(bytes.data() + 1, bytes.data(), bytes.size() - 1);
  for (std::size_t i = 0; i < bytes.size(); ++i) {
    CHECK(nstd::to_integer(bytes[i]) == (i == 0 ? 0 : i - 1));
  }

  nstd::from_bytes(bytes.data(), bytes.data() + 1, bytes.size() - 1);
  for (std::size_t i = 0; i < bytes.size(); ++i) {
    const auto expected = i < bytes.size() - 1 ? i : bytes.size() - 2;
    CHECK(nstd::to_integer(bytes[i]) == expected);
  }

  const auto unchanged = bytes;
  nstd::to_bytes(bytes.data(), bytes.data(), bytes.size());
  nstd::from_bytes(bytes.data(), bytes.data(), bytes.size());
  CHECK(bytes == unchanged);
}
