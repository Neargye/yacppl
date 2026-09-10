// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <utility.hpp>

#include "representation_test_types.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

struct tuple_size_only {};

namespace std {

template <>
struct tuple_size<tuple_size_only> : integral_constant<size_t, 1> {};

} // namespace std

namespace constexpr_for_adl_test {

struct callback {
  int& sum;

  template <typename T>
  constexpr void operator()(T index) const {
    sum += index;
  }
};

template <auto Start, auto End, auto Inc>
void constexpr_for(callback) = delete;

} // namespace constexpr_for_adl_test

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

struct copy_only_constructible {
  copy_only_constructible() = default;
  copy_only_constructible(const copy_only_constructible&) noexcept = default;
  copy_only_constructible(copy_only_constructible&&) = delete;
};

struct throwing_move_copyable {
  throwing_move_copyable() = default;
  throwing_move_copyable(const throwing_move_copyable&) noexcept = default;
  throwing_move_copyable(throwing_move_copyable&&) noexcept(false) {}
};

struct throwing_move_only {
  throwing_move_only() = default;
  throwing_move_only(const throwing_move_only&) = delete;
  throwing_move_only(throwing_move_only&&) noexcept(false) {}
};

struct nothrow_move_copyable {
  nothrow_move_copyable() = default;
  nothrow_move_copyable(const nothrow_move_copyable&) noexcept = default;
  nothrow_move_copyable(nothrow_move_copyable&&) noexcept = default;
};

struct throwing_move_only_assignable {
  throwing_move_only_assignable& operator=(const throwing_move_only_assignable&) = delete;
  throwing_move_only_assignable& operator=(throwing_move_only_assignable&&) noexcept(false) {
    return *this;
  }
};

struct nothrow_move_assignable {
  nothrow_move_assignable& operator=(const nothrow_move_assignable&) = default;
  nothrow_move_assignable& operator=(nothrow_move_assignable&&) noexcept = default;
};

struct explicit_materializable {
  int value = 0;

  explicit_materializable() = default;
  explicit_materializable(const explicit_materializable&) = delete;

  template <typename U, typename std::enable_if<std::is_same<typename std::remove_reference<U>::type, explicit_materializable>::value, int>::type = 0>
  explicit explicit_materializable(U&& other) noexcept : value{other.value} {}
};

struct member_target {
  int calls = 0;

  void touch() noexcept {
    ++calls;
  }
};

struct nothrow_callable {
  void operator()(int) const noexcept {}
};

struct throwing_callable {
  void operator()(int) const noexcept(false) {}
};

struct incompatible_callable {
  void operator()(int*) const noexcept {}
};

struct any_index_callable {
  template <typename I>
  void operator()(I) const noexcept {}
};

struct only_zero_callable {
  void operator()(std::integral_constant<int, 0>) const noexcept {}
};

int sample_function(double) noexcept {
  return 0;
}

struct byte_array {
  unsigned char value[sizeof(std::uint32_t)];
};

struct nontrivial_copy {
  std::uint32_t value;

  nontrivial_copy() = default;
  nontrivial_copy(const nontrivial_copy& other) : value{other.value} {}
};

struct nonreturnable_trivial {
  std::uint32_t value;

  nonreturnable_trivial() = default;
  nonreturnable_trivial(const nonreturnable_trivial&) = delete;
  nonreturnable_trivial(nonreturnable_trivial&&) = delete;
  nonreturnable_trivial& operator=(const nonreturnable_trivial&) = default;
};

struct nontrivial_default_trivial {
  std::uint32_t value;

  nontrivial_default_trivial() noexcept : value{} {}
};

struct potentially_throwing_default_trivial {
  std::uint32_t value;

  potentially_throwing_default_trivial() noexcept(false) : value{} {}
};

struct potentially_throwing_return_trivial {
  std::uint32_t value;

  potentially_throwing_return_trivial() noexcept = default;
  potentially_throwing_return_trivial(const potentially_throwing_return_trivial&) = default;

  template <typename U, typename std::enable_if<std::is_same<typename std::remove_reference<U>::type, potentially_throwing_return_trivial>::value && !std::is_lvalue_reference<U>::value, int>::type = 0>
  potentially_throwing_return_trivial(U&& other) noexcept(false) : value{other.value + 1} {
    throw 1;
  }
};

struct explicit_only_return_trivial {
  std::uint32_t value;

  explicit_only_return_trivial() = default;
  explicit_only_return_trivial(const explicit_only_return_trivial&) = delete;

  template <typename U, typename std::enable_if<std::is_same<typename std::remove_reference<U>::type, explicit_only_return_trivial>::value, int>::type = 0>
  explicit explicit_only_return_trivial(U&& other) noexcept : value{other.value} {}
};

struct explicit_throwing_return_trivial {
  std::uint32_t value;

  explicit_throwing_return_trivial() = default;
  explicit_throwing_return_trivial(const explicit_throwing_return_trivial&) = default;

  template <typename U, typename std::enable_if<std::is_same<typename std::remove_reference<U>::type, explicit_throwing_return_trivial>::value && !std::is_lvalue_reference<U>::value, int>::type = 0>
  explicit explicit_throwing_return_trivial(U&& other) noexcept(false) : value{other.value} {}
};

template <typename To, typename From, typename = void>
struct can_bit_cast : std::false_type {};

template <typename To, typename From>
struct can_bit_cast<To, From, std::void_t<decltype(nstd::bit_cast<To>(std::declval<const From&>()))>> : std::true_type {};

template <typename T, typename = void>
struct can_move : std::false_type {};

template <typename T>
struct can_move<T, std::void_t<decltype(nstd::move(std::declval<T>()))>> : std::true_type {};

template <typename T, typename = void>
struct can_move_if_noexcept : std::false_type {};

template <typename T>
struct can_move_if_noexcept<T, std::void_t<decltype(nstd::move_if_noexcept(std::declval<T&>()))>> : std::true_type {};

template <typename T, typename = void>
struct can_move_assign_if_noexcept : std::false_type {};

template <typename T>
struct can_move_assign_if_noexcept<T, std::void_t<decltype(nstd::move_assign_if_noexcept(std::declval<T&>()))>> : std::true_type {};

template <typename T, typename = void>
struct can_forward_rvalue : std::false_type {};

template <typename T>
struct can_forward_rvalue<T, std::void_t<decltype(nstd::forward<T>(std::declval<std::remove_reference_t<T>&&>()))>> : std::true_type {};

template <typename T, typename = void>
struct can_unforward : std::false_type {};

template <typename T>
struct can_unforward<T, std::void_t<decltype(nstd::unforward(std::declval<T>()))>> : std::true_type {};

template <typename T, typename = void>
struct can_decay_copy : std::false_type {};

template <typename T>
struct can_decay_copy<T, std::void_t<decltype(nstd::decay_copy(std::declval<T>()))>> : std::true_type {};

template <typename Model, typename T, typename = void>
struct can_forward_like : std::false_type {};

template <typename Model, typename T>
struct can_forward_like<Model, T, std::void_t<decltype(nstd::forward_like<Model>(std::declval<T>()))>> : std::true_type {};

template <typename F, typename... Args>
using invoke_each_result_t = decltype(nstd::invoke_each(std::declval<F>(), std::declval<Args>()...));

template <typename F, typename Tuple>
using apply_each_result_t = decltype(nstd::apply_each(std::declval<F>(), std::declval<Tuple>()));

template <template <typename...> class Op, typename, typename... Args>
struct is_detected_impl : std::false_type {};

template <template <typename...> class Op, typename... Args>
struct is_detected_impl<Op, std::void_t<Op<Args...>>, Args...> : std::true_type {};

template <template <typename...> class Op, typename... Args>
using is_detected = is_detected_impl<Op, void, Args...>;

template <typename T, typename U, typename = void>
struct can_cmp_equal : std::false_type {};

template <typename T, typename U>
struct can_cmp_equal<T, U, std::void_t<decltype(nstd::cmp_equal(std::declval<T>(), std::declval<U>()))>> : std::true_type {};

template <typename R, typename T, typename = void>
struct can_in_range : std::false_type {};

template <typename R, typename T>
struct can_in_range<R, T, std::void_t<decltype(nstd::in_range<R>(std::declval<T>()))>> : std::true_type {};

template <auto Start, auto End, auto Inc, typename F, typename = void>
struct can_constexpr_for : std::false_type {};

template <auto Start, auto End, auto Inc, typename F>
struct can_constexpr_for<Start, End, Inc, F,
                         std::void_t<decltype(nstd::constexpr_for<Start, End, Inc>(std::declval<F>()))>>
    : std::true_type {};

template <typename... T>
struct type_list {};

template <typename T>
void check_same_type_integer_boundaries() {
  constexpr auto min = (std::numeric_limits<T>::min)();
  constexpr auto max = (std::numeric_limits<T>::max)();

  CHECK(nstd::cmp_equal(min, min));
  CHECK(nstd::cmp_equal(max, max));
  CHECK(nstd::cmp_less(min, max));
  CHECK(nstd::in_range<T>(min));
  CHECK(nstd::in_range<T>(max));
}

template <typename Signed, typename Unsigned>
void check_mixed_integer_boundaries() {
  static_assert(std::is_signed<Signed>::value, "Signed must be signed.");
  static_assert(std::is_unsigned<Unsigned>::value, "Unsigned must be unsigned.");

  constexpr auto signed_min = (std::numeric_limits<Signed>::min)();
  constexpr auto signed_max = (std::numeric_limits<Signed>::max)();
  constexpr auto unsigned_max = (std::numeric_limits<Unsigned>::max)();
  constexpr bool maxima_equal = std::numeric_limits<Signed>::digits == std::numeric_limits<Unsigned>::digits;
  constexpr bool signed_max_less = std::numeric_limits<Signed>::digits < std::numeric_limits<Unsigned>::digits;
  constexpr bool unsigned_fits_signed = std::numeric_limits<Unsigned>::digits <= std::numeric_limits<Signed>::digits;
  constexpr bool signed_fits_unsigned = std::numeric_limits<Signed>::digits <= std::numeric_limits<Unsigned>::digits;

  CHECK(nstd::cmp_less(signed_min, Unsigned{0}));
  CHECK_FALSE(nstd::cmp_equal(signed_min, unsigned_max));
  CHECK(nstd::cmp_equal(Signed{0}, Unsigned{0}));
  CHECK(nstd::cmp_equal(signed_max, unsigned_max) == maxima_equal);
  CHECK(nstd::cmp_less(signed_max, unsigned_max) == signed_max_less);
  CHECK(nstd::cmp_greater(unsigned_max, signed_max) == signed_max_less);
  CHECK_FALSE(nstd::in_range<Unsigned>(signed_min));
  CHECK(nstd::in_range<Signed>(unsigned_max) == unsigned_fits_signed);
  CHECK(nstd::in_range<Unsigned>(signed_max) == signed_fits_unsigned);
}

template <typename Signed, typename... Unsigned>
void check_unsigned_rows(type_list<Unsigned...>) {
  (check_mixed_integer_boundaries<Signed, Unsigned>(), ...);
}

template <typename... Signed, typename... Unsigned>
void check_all_mixed_integer_boundaries(type_list<Signed...>, type_list<Unsigned...> unsigned_types) {
  (check_unsigned_rows<Signed>(unsigned_types), ...);
}

template <typename... T>
void check_all_same_type_integer_boundaries(type_list<T...>) {
  (check_same_type_integer_boundaries<T>(), ...);
}

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
  int values[] = {1, 2};
  copy_only_assignable copy_only;
  throwing_move_assignable throwing_move;
  throwing_move_only_assignable throwing_move_only_assignment;
  nothrow_move_assignable nothrow_move_assignment;

  static_assert(std::is_same<decltype(nstd::move(value)), int&&>::value, "move must produce an rvalue reference.");
  static_assert(std::is_same<decltype(nstd::move(values)), int (&&)[2]>::value, "move must preserve array type.");
  static_assert(std::is_same<decltype(nstd::move(sample_function)), decltype(std::move(sample_function))>::value, "move must match std::move for function types.");
  static_assert(!can_move<const int&>::value, "move must reject const objects through SFINAE.");
  static_assert(!can_move_if_noexcept<const int>::value, "move_if_noexcept must reject const objects through SFINAE.");
  static_assert(!can_move_assign_if_noexcept<const int>::value, "move_assign_if_noexcept must reject const objects through SFINAE.");

  static_assert(std::is_same<decltype(nstd::move_if_noexcept(std::declval<copy_only_constructible&>())), const copy_only_constructible&>::value, "copy-only construction must select const lvalue.");
  static_assert(std::is_same<decltype(nstd::move_if_noexcept(std::declval<throwing_move_copyable&>())), const throwing_move_copyable&>::value, "throwing move with copy fallback must select const lvalue.");
  static_assert(std::is_same<decltype(nstd::move_if_noexcept(std::declval<throwing_move_only&>())), throwing_move_only&&>::value, "move-only construction must select rvalue even when it may throw.");
  static_assert(std::is_same<decltype(nstd::move_if_noexcept(std::declval<nothrow_move_copyable&>())), nothrow_move_copyable&&>::value, "nothrow move construction must select rvalue.");

  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(copy_only)), const copy_only_assignable&>::value, "copy-only assignment must select const lvalue.");
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(throwing_move)), const throwing_move_assignable&>::value, "throwing move assignment with copy fallback must select const lvalue.");
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(throwing_move_only_assignment)), throwing_move_only_assignable&&>::value, "move-only assignment must select rvalue even when it may throw.");
  static_assert(std::is_same<decltype(nstd::move_assign_if_noexcept(nothrow_move_assignment)), nothrow_move_assignable&&>::value, "nothrow move assignment must select rvalue.");

  static_assert(can_forward_rvalue<int>::value, "forward<int> must accept an rvalue.");
  static_assert(!can_forward_rvalue<int&>::value, "forward<int&> must reject an rvalue through SFINAE.");
  static_assert(std::is_same<decltype(nstd::unforward(std::declval<const int&>())), int>::value, "scalar prvalues discard top-level const.");
  static_assert(can_unforward<explicit_materializable&>::value, "unforward must use direct construction.");
  static_assert(can_decay_copy<explicit_materializable&>::value, "decay_copy must use direct construction.");
  static_assert(!can_unforward<int (&)[2]>::value, "unforward must reject non-materializable arrays.");
  static_assert(!can_unforward<int (&)(double) noexcept>::value, "unforward must reject function types.");
  static_assert(std::is_same<decltype(nstd::decay_copy(values)), int*>::value, "decay_copy must decay arrays to pointers.");
  static_assert(std::is_same<decltype(nstd::decay_copy(sample_function)), int (*)(double) noexcept>::value, "decay_copy must decay functions to pointers.");
  static_assert(noexcept(nstd::unforward(std::declval<std::unique_ptr<int>>())), "unforward must reflect nothrow move construction.");

  CHECK(nstd::unforward(value) == 1);
  CHECK(nstd::decay_copy(value) == 1);
  CHECK(nstd::decay_copy(values) == values);
  CHECK(nstd::decay_copy(sample_function)(0.0) == 0);

  explicit_materializable materializable;
  materializable.value = 7;
  CHECK(nstd::unforward(materializable).value == 7);
  CHECK(nstd::decay_copy(materializable).value == 7);
}

TEST_CASE("move and materialization helpers work with standard library types") {
  std::string text = "saved";
  auto&& moved = nstd::move_if_noexcept(text);
  auto&& assigned = nstd::move_assign_if_noexcept(text);

  CHECK(std::addressof(moved) == std::addressof(text));
  CHECK(std::addressof(assigned) == std::addressof(text));
  CHECK(nstd::unforward(text) == "saved");
  CHECK(nstd::decay_copy(text) == "saved");
  CHECK(nstd::unforward(std::string{"temporary"}) == "temporary");
  CHECK(nstd::decay_copy(std::string{"temporary"}) == "temporary");

  auto owner = std::make_unique<int>(7);
  auto result = nstd::unforward(std::move(owner));
  CHECK(owner == nullptr);
  CHECK(*result == 7);
  auto copy = nstd::decay_copy(std::move(result));
  CHECK(result == nullptr);
  CHECK(*copy == 7);
}

TEST_CASE("bit_cast bypasses overloaded addresses and non-trivial return constructors") {
  using namespace representation_test;
  static_assert(!can_bit_cast<unsafe_return, std::uint32_t>::value, "a non-trivial return constructor must be rejected.");

  const std::uint32_t value = 7;
  address_trap::unrelated = 99;
  const auto source = nstd::bit_cast<address_trap>(value);
  CHECK(source.value == value);
  CHECK(nstd::bit_cast<std::uint32_t>(source) == value);
  CHECK(address_trap::unrelated == 99);

  const auto no_address = nstd::bit_cast<deleted_address>(value);
  CHECK(nstd::bit_cast<std::uint32_t>(no_address) == value);
  CHECK(nstd::bit_cast<copy_only>(value).value == value);
  CHECK(nstd::bit_cast<move_only>(value).value == value);
  CHECK(nstd::bit_cast<explicit_copy>(value).value == value);
}

TEST_CASE("bit_cast round-trips trivially copyable values") {
  static_assert(noexcept(nstd::bit_cast<byte_array>(std::declval<const std::uint32_t&>())), "bit_cast must be noexcept for nothrow construction.");
  static_assert(std::is_trivially_copyable<nonreturnable_trivial>::value, "test type must be trivially copyable.");
  static_assert(std::is_trivially_default_constructible<nonreturnable_trivial>::value, "test type must be trivially default constructible.");
  static_assert(!std::is_constructible<nonreturnable_trivial, nonreturnable_trivial&&>::value, "test type must not be returnable by value.");
  static_assert(!can_bit_cast<nonreturnable_trivial, std::uint32_t>::value, "bit_cast must reject types that cannot be returned by value.");
  static_assert(!std::is_trivially_copyable<nontrivial_copy>::value, "test type must not be trivially copyable.");
  static_assert(!can_bit_cast<byte_array, nontrivial_copy>::value, "bit_cast must reject non-trivially-copyable sources.");
  static_assert(!can_bit_cast<std::uint64_t, std::uint32_t>::value, "bit_cast must reject different sizes.");
  static_assert(std::is_trivially_copyable<nontrivial_default_trivial>::value, "test type must be trivially copyable.");
  static_assert(!std::is_trivially_default_constructible<nontrivial_default_trivial>::value, "test type must have a non-trivial default constructor.");
  static_assert(can_bit_cast<nontrivial_default_trivial, std::uint32_t>::value, "bit_cast should accept an ordinary default constructor.");
  static_assert(std::is_trivially_copyable<potentially_throwing_default_trivial>::value, "test type must be trivially copyable.");
  static_assert(can_bit_cast<potentially_throwing_default_trivial, std::uint32_t>::value, "bit_cast should accept a potentially throwing default constructor.");
  static_assert(!noexcept(nstd::bit_cast<potentially_throwing_default_trivial>(std::declval<const std::uint32_t&>())), "bit_cast must reflect a potentially throwing default constructor.");
  static_assert(std::is_trivially_copyable<potentially_throwing_return_trivial>::value, "test type must be trivially copyable.");
  static_assert(can_bit_cast<potentially_throwing_return_trivial, std::uint32_t>::value, "bit_cast should accept a returnable type.");
  static_assert(noexcept(nstd::bit_cast<potentially_throwing_return_trivial>(std::declval<const std::uint32_t&>())), "bit_cast must use the trivial copy constructor.");
  static_assert(std::is_trivially_copyable<explicit_only_return_trivial>::value, "test type must be trivially copyable.");
  static_assert(std::is_constructible<explicit_only_return_trivial, explicit_only_return_trivial&&>::value, "test type must expose the constructible/convertible distinction.");
  static_assert(!std::is_convertible<explicit_only_return_trivial&&, explicit_only_return_trivial>::value, "test type must not support implicit return construction.");
  static_assert(!can_bit_cast<explicit_only_return_trivial, std::uint32_t>::value, "bit_cast must reject a type that only has an explicit return constructor.");
  static_assert(std::is_trivially_copyable<explicit_throwing_return_trivial>::value, "test type must be trivially copyable.");
  static_assert(!std::is_nothrow_constructible<explicit_throwing_return_trivial, explicit_throwing_return_trivial&&>::value, "direct construction must select the explicit throwing constructor.");
  static_assert(std::is_convertible<explicit_throwing_return_trivial&&, explicit_throwing_return_trivial>::value, "implicit return construction must remain available through the copy constructor.");
  static_assert(can_bit_cast<explicit_throwing_return_trivial, std::uint32_t>::value, "bit_cast should accept an implicitly returnable type.");
  static_assert(noexcept(nstd::bit_cast<explicit_throwing_return_trivial>(std::declval<const std::uint32_t&>())), "bit_cast must avoid the non-trivial rvalue constructor.");
  static_assert(!can_bit_cast<const byte_array, std::uint32_t>::value, "bit_cast must reject const destinations that memcpy cannot write.");
  static_assert(!can_bit_cast<volatile byte_array, std::uint32_t>::value, "bit_cast must reject volatile destinations that memcpy cannot write.");
  static_assert(!can_bit_cast<byte_array, volatile std::uint32_t>::value, "bit_cast must reject volatile sources that memcpy cannot access.");

  const std::uint32_t value = 0x12345678u;

  const auto bytes = nstd::bit_cast<byte_array>(value);
  const auto roundtrip = nstd::bit_cast<std::uint32_t>(bytes);
  const auto custom_default = nstd::bit_cast<nontrivial_default_trivial>(value);
  const auto throwing_default = nstd::bit_cast<potentially_throwing_default_trivial>(value);
  const auto throwing_return = nstd::bit_cast<potentially_throwing_return_trivial>(value);
  const auto explicit_return = nstd::bit_cast<explicit_throwing_return_trivial>(value);

  CHECK(roundtrip == value);
  CHECK(custom_default.value == value);
  CHECK(throwing_default.value == value);
  CHECK(throwing_return.value == value);
  CHECK(explicit_return.value == value);
}

TEST_CASE("invoke_each invokes the same callable as an lvalue") {
  int sum = 0;
  member_target first;
  member_target second;

  static_assert(is_detected<invoke_each_result_t, nothrow_callable, int>::value, "valid invoke_each call must be detected.");
  static_assert(!is_detected<invoke_each_result_t, incompatible_callable, int>::value, "invalid invoke_each call must be rejected through SFINAE.");
  static_assert(noexcept(nstd::invoke_each(nothrow_callable{}, 1, 2)), "invoke_each must be noexcept when every invocation is noexcept.");
  static_assert(!noexcept(nstd::invoke_each(throwing_callable{}, 1)), "invoke_each must reflect throwing invocation.");

  nstd::invoke_each([](int) {});
  nstd::invoke_each(lvalue_only_callable{&sum}, 1, 2, 3);
  nstd::invoke_each(&member_target::touch, std::ref(first), &second);
  nstd::invoke_each([&sum](std::unique_ptr<int> item) { sum += *item; },
                    std::make_unique<int>(4), std::make_unique<int>(5));

  CHECK(sum == 15);
  CHECK(first.calls == 1);
  CHECK(second.calls == 1);
}

TEST_CASE("apply_each supports tuple-like lvalues, const lvalues, and rvalues") {
  int sum = 0;
  auto empty = std::make_tuple();
  auto values = std::make_tuple(1, 2, 3);
  const auto const_values = std::make_pair(4, 5);
  std::array<int, 2> array_values{{6, 7}};
  member_target first;
  member_target second;

  static_assert(is_detected<apply_each_result_t, nothrow_callable, std::tuple<int>>::value, "valid apply_each call must be detected.");
  static_assert(!is_detected<apply_each_result_t, incompatible_callable, std::tuple<int>>::value, "invalid callable must be rejected through SFINAE.");
  static_assert(!is_detected<apply_each_result_t, nothrow_callable, int>::value, "non-tuple input must be rejected through SFINAE.");
  static_assert(!is_detected<apply_each_result_t, nothrow_callable, tuple_size_only>::value, "tuple_size without matching get overloads must be rejected through SFINAE.");
  static_assert(noexcept(nstd::apply_each(nothrow_callable{}, std::declval<std::tuple<int>&>())), "apply_each must be noexcept when every invocation is noexcept.");
  static_assert(!noexcept(nstd::apply_each(throwing_callable{}, std::declval<std::tuple<int>&>())), "apply_each must reflect throwing invocation.");

  nstd::apply_each([](int) {}, empty);
  nstd::apply_each([&sum](int value) { sum += value; }, values);
  nstd::apply_each([&sum](int value) { sum += value; }, const_values);
  nstd::apply_each([&sum](int value) { sum += value; }, array_values);
  nstd::apply_each([&sum](std::unique_ptr<int> value) { sum += *value; },
                   std::make_tuple(std::unique_ptr<int>{new int{8}}));
  nstd::apply_each(&member_target::touch, std::tie(first, second));

  CHECK(sum == 36);
  CHECK(first.calls == 1);
  CHECK(second.calls == 1);
}

TEST_CASE("forward_like applies cv-ref qualifiers from the model type") {
  int value = 42;
  volatile int volatile_value = 42;
  int values[] = {1, 2};

  static_assert(std::is_same<decltype(nstd::forward_like<int&>(value)), int&>::value, "");
  static_assert(std::is_same<decltype(nstd::forward_like<const int&>(value)), const int&>::value, "");
  static_assert(std::is_same<decltype(nstd::forward_like<volatile int&&>(value)), int&&>::value, "forward_like must not copy volatile from its model type.");
  static_assert(std::is_same<decltype(nstd::forward_like<const volatile int>(value)), const int&&>::value, "forward_like must copy const but not volatile.");
  static_assert(std::is_same<decltype(nstd::forward_like<int&>(volatile_value)), volatile int&>::value, "forward_like must preserve qualifiers already present on the source.");
  static_assert(std::is_same<decltype(nstd::forward_like<const int&>(values)), const int (&)[2]>::value, "forward_like must preserve array type.");
  static_assert(std::is_same<decltype(nstd::forward_like<int&>(sample_function)), int (&)(double) noexcept>::value, "forward_like must preserve function type.");
  static_assert(!can_forward_like<void, int&>::value, "forward_like must reject a non-referenceable model type.");
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

  static_assert(!can_cmp_equal<bool, int>::value, "C++20 comparison helpers must reject bool.");
  static_assert(!can_cmp_equal<char, int>::value, "C++20 comparison helpers must reject char.");
  static_assert(!can_cmp_equal<wchar_t, int>::value, "C++20 comparison helpers must reject wchar_t.");
  static_assert(!can_cmp_equal<char16_t, int>::value, "C++20 comparison helpers must reject char16_t.");
  static_assert(!can_cmp_equal<char32_t, int>::value, "C++20 comparison helpers must reject char32_t.");
#if defined(__cpp_char8_t)
  static_assert(!can_cmp_equal<char8_t, int>::value, "C++20 comparison helpers must reject char8_t.");
#endif

  using signed_types = type_list<signed char, short, int, long, long long>;
  using unsigned_types = type_list<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;
  check_all_same_type_integer_boundaries(signed_types{});
  check_all_same_type_integer_boundaries(unsigned_types{});
  check_all_mixed_integer_boundaries(signed_types{}, unsigned_types{});
}

TEST_CASE("in_range checks whether integer values fit in the target type") {
  constexpr auto above_signed_char_max =
      static_cast<unsigned int>((std::numeric_limits<signed char>::max)()) + 1u;

  static_assert(nstd::in_range<unsigned char>(0), "");
  static_assert(nstd::in_range<unsigned char>((std::numeric_limits<unsigned char>::max)()), "");
  static_assert(!nstd::in_range<unsigned char>(-1), "");
  static_assert(!nstd::in_range<signed char>(above_signed_char_max), "");
  static_assert(nstd::in_range<int>(42u), "");

  CHECK(nstd::in_range<unsigned char>((std::numeric_limits<unsigned char>::max)()));
  CHECK_FALSE(nstd::in_range<unsigned char>(-1));
  CHECK_FALSE(nstd::in_range<signed char>(above_signed_char_max));

  static_assert(!can_in_range<bool, int>::value, "in_range must reject bool result types.");
  static_assert(!can_in_range<int, char>::value, "in_range must reject character input types.");
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

  constexpr int negative_sum = [] {
    int value = 0;
    nstd::constexpr_for<-3, 2, 2>([&value](auto index) {
      value += index;
    });
    return value;
  }();
  constexpr int overflow_safe_count = [] {
    int count = 0;
    nstd::constexpr_for<(std::numeric_limits<int>::max)() - 1,
                        (std::numeric_limits<int>::max)(), 2>([&count](auto) {
      ++count;
    });
    return count;
  }();
  constexpr int narrow_sum = [] {
    int value = 0;
    nstd::constexpr_for<static_cast<signed char>(0),
                        static_cast<signed char>(4),
                        static_cast<signed char>(1)>([&value](auto index) {
      value += index;
    });
    return value;
  }();

  static_assert(negative_sum == -3, "constexpr_for must support negative starts.");
  static_assert(overflow_safe_count == 1, "constexpr_for must stop without overflowing Start + Inc.");
  static_assert(narrow_sum == 6, "constexpr_for must preserve narrow integral parameter types.");
  static_assert(can_constexpr_for<0, 1, 1, only_zero_callable>::value, "callable valid for every iteration must be accepted.");
  static_assert(!can_constexpr_for<0, 2, 1, only_zero_callable>::value, "callable invalid for a later iteration must be rejected through SFINAE.");
  static_assert(can_constexpr_for<1, 1, 1, incompatible_callable>::value, "an empty range must not require an invocable callable.");
  static_assert(!can_constexpr_for<0, 1, 0, any_index_callable>::value, "zero increment must be rejected through SFINAE.");
  static_assert(!can_constexpr_for<0, 1, -1, any_index_callable>::value, "negative increment must be rejected through SFINAE.");
  static_assert(!can_constexpr_for<0, 1u, 1, any_index_callable>::value, "mixed parameter types must be rejected through SFINAE.");
  static_assert(!can_constexpr_for<false, true, true, any_index_callable>::value, "bool loop parameters must be rejected through SFINAE.");
  static_assert(!can_constexpr_for<nullptr, nullptr, nullptr, any_index_callable>::value, "non-integral parameters must not instantiate arithmetic comparisons.");
  static_assert(!can_constexpr_for<0, 1, 1u, any_index_callable>::value, "the increment must have the same type as the bounds.");

  using member_callable = decltype(&std::integral_constant<int, 0>::operator());
  static_assert(std::is_invocable_v<member_callable, std::integral_constant<int, 0>>, "test must distinguish INVOKE from a direct call.");
  static_assert(!can_constexpr_for<0, 1, 1, member_callable>::value, "member pointers cannot be called directly and must be rejected through SFINAE.");
  using last_member_callable = decltype(&std::integral_constant<int, (std::numeric_limits<int>::max)() - 1>::operator());
  static_assert(!can_constexpr_for<(std::numeric_limits<int>::max)() - 1, (std::numeric_limits<int>::max)(), 2, last_member_callable>::value,
                "the overflow-stopping iteration must use the same direct-call constraint.");
  static_assert(can_constexpr_for<1, 1, 1, member_callable>::value, "empty ranges do not call their callback.");

  CHECK(negative_sum == -3);
  CHECK(overflow_safe_count == 1);
  CHECK(narrow_sum == 6);
}

TEST_CASE("constexpr_for recursion ignores unrelated ADL overloads") {
  constexpr int sum = [] {
    int value = 0;
    nstd::constexpr_for<0, 4, 1>(constexpr_for_adl_test::callback{value});
    return value;
  }();

  static_assert(sum == 6, "every iteration must use nstd::constexpr_for.");
  CHECK(sum == 6);
}
