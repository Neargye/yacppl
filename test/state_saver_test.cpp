// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#if defined(STATE_SAVER_TEST_CXXABI_FIRST) && !defined(_MSC_VER) && __cplusplus < 201703L && defined(__GXX_ABI_VERSION)
#  include <cxxabi.h>
#endif

#include <state_saver.hpp>

#if !defined(STATE_SAVER_TEST_CXXABI_FIRST) && !defined(_MSC_VER) && __cplusplus < 201703L && defined(__GXX_ABI_VERSION)
#  include <cxxabi.h>
#endif

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

constexpr int original_value = 1;
constexpr int changed_value = 2;

struct both_assignable {
  int value;

  explicit both_assignable(int value) : value{value} {}
  both_assignable(const both_assignable&) = default;
  both_assignable(both_assignable&&) = default;
  both_assignable& operator=(const both_assignable&) = default;
  both_assignable& operator=(both_assignable&&) = default;
};

struct copy_assignable_only {
  int value;

  explicit copy_assignable_only(int value) : value{value} {}
  copy_assignable_only(const copy_assignable_only&) = default;
  copy_assignable_only(copy_assignable_only&&) = default;
  copy_assignable_only& operator=(const copy_assignable_only&) = default;
  copy_assignable_only& operator=(copy_assignable_only&&) = delete;
};

struct move_assignable_only {
  int value;

  explicit move_assignable_only(int value) : value{value} {}
  move_assignable_only(const move_assignable_only&) = default;
  move_assignable_only(move_assignable_only&&) = default;
  move_assignable_only& operator=(const move_assignable_only&) = delete;
  move_assignable_only& operator=(move_assignable_only&&) = default;
};

struct throwing_copy_assignable {
  int value;

  explicit throwing_copy_assignable(int value) : value{value} {}
  throwing_copy_assignable(const throwing_copy_assignable&) = default;
  throwing_copy_assignable(throwing_copy_assignable&&) = default;

  throwing_copy_assignable& operator=(const throwing_copy_assignable&) {
    throw std::runtime_error{"copy assignment"};
  }

  throwing_copy_assignable& operator=(throwing_copy_assignable&& other) noexcept {
    value = other.value;
    return *this;
  }
};

struct throwing_move_assignable {
  int value;

  explicit throwing_move_assignable(int value) : value{value} {}
  throwing_move_assignable(const throwing_move_assignable&) = default;
  throwing_move_assignable(throwing_move_assignable&&) = default;
  throwing_move_assignable& operator=(const throwing_move_assignable&) = default;

  throwing_move_assignable& operator=(throwing_move_assignable&& other) {
    value = other.value;
    throw std::runtime_error{"move assignment"};
  }
};

struct lvalue_snapshot {
  int value;

  explicit lvalue_snapshot(int value) : value{value} {}
  lvalue_snapshot(lvalue_snapshot& other) noexcept : value{other.value} {}
  lvalue_snapshot(const lvalue_snapshot& other) noexcept(false) : value{other.value} {}
  lvalue_snapshot(lvalue_snapshot&&) noexcept = default;
  lvalue_snapshot& operator=(const lvalue_snapshot&) noexcept = default;
  lvalue_snapshot& operator=(lvalue_snapshot&&) noexcept = default;
};

struct initializer_list_snapshot {
  int value;

  explicit initializer_list_snapshot(int value) noexcept : value(value) {}
  initializer_list_snapshot(const initializer_list_snapshot&) noexcept = default;
  initializer_list_snapshot& operator=(const initializer_list_snapshot&) noexcept = default;
  initializer_list_snapshot(std::initializer_list<initializer_list_snapshot>) {
    throw std::runtime_error{"initializer-list construction must not be used"};
  }
};

struct both_throwing_assignable {
  static int copy_assign_count;
  static int move_assign_count;

  int value;

  explicit both_throwing_assignable(int value) : value{value} {}
  both_throwing_assignable(const both_throwing_assignable&) = default;
  both_throwing_assignable(both_throwing_assignable&&) = default;

  both_throwing_assignable& operator=(const both_throwing_assignable& other) noexcept(false) {
    ++copy_assign_count;
    value = other.value;
    return *this;
  }

  both_throwing_assignable& operator=(both_throwing_assignable&& other) noexcept(false) {
    ++move_assign_count;
    value = other.value;
    return *this;
  }
};

int both_throwing_assignable::copy_assign_count = 0;
int both_throwing_assignable::move_assign_count = 0;

struct always_throwing_assignable {
  int value;

  explicit always_throwing_assignable(int value) : value{value} {}
  always_throwing_assignable(const always_throwing_assignable&) = default;
  always_throwing_assignable(always_throwing_assignable&&) = default;

  always_throwing_assignable& operator=(const always_throwing_assignable&) {
    throw std::runtime_error{"lvalue assignment"};
  }

  always_throwing_assignable& operator=(always_throwing_assignable&&) {
    throw std::runtime_error{"rvalue assignment"};
  }
};

struct unwinding_probe {
  int& same_fail;
  int& same_success;
  int& nested_fail;
  int& nested_success;

  unwinding_probe(int& same_fail,
                  int& same_success,
                  int& nested_fail,
                  int& nested_success) noexcept
      : same_fail{same_fail},
        same_success{same_success},
        nested_fail{nested_fail},
        nested_success{nested_success} {}

  ~unwinding_probe() noexcept {
    {
      nstd::saver_fail<int&> saver{same_fail};
      same_fail = changed_value;
    }

    {
      nstd::saver_success<int&> saver{same_success};
      same_success = changed_value;
    }

    try {
      nstd::saver_fail<int&> saver{nested_fail};
      nested_fail = changed_value;
      throw std::runtime_error{"nested failure"};
    } catch (const std::runtime_error&) {
    }

    try {
      nstd::saver_success<int&> saver{nested_success};
      nested_success = changed_value;
      throw std::runtime_error{"nested failure"};
    } catch (const std::runtime_error&) {
    }
  }
};

template <typename...>
struct make_void {
  using type = void;
};

template <typename T, typename = void>
struct has_restore : std::false_type {};

template <typename T>
struct has_restore<T, typename make_void<decltype(std::declval<T&>().restore())>::type> : std::true_type {};

template <typename T, bool = has_restore<T>::value>
struct has_nothrow_restore : std::false_type {};

template <typename T>
struct has_nothrow_restore<T, true>
    : std::integral_constant<bool, noexcept(std::declval<T&>().restore())> {};

} // namespace

TEST_CASE_TEMPLATE("saver_exit restores after normal and exceptional exits",
                   T,
                   both_assignable,
                   copy_assignable_only,
                   move_assignable_only,
                   throwing_copy_assignable,
                   throwing_move_assignable) {
  T value{original_value};

  {
    nstd::saver_exit<T&> saver{value};
    value.value = changed_value;
  }
  CHECK(value.value == original_value);

  CHECK_THROWS([&]() {
    nstd::saver_exit<T&> saver{value};
    value.value = changed_value;
    throw std::runtime_error{"error"};
  }());
  CHECK(value.value == original_value);
}

TEST_CASE_TEMPLATE("saver_fail restores only after exceptional exits",
                   T,
                   both_assignable,
                   copy_assignable_only,
                   move_assignable_only,
                   throwing_copy_assignable,
                   throwing_move_assignable) {
  T value{original_value};

  {
    nstd::saver_fail<T&> saver{value};
    value.value = changed_value;
  }
  CHECK(value.value == changed_value);

  value.value = original_value;
  CHECK_THROWS([&]() {
    nstd::saver_fail<T&> saver{value};
    value.value = changed_value;
    throw std::runtime_error{"error"};
  }());
  CHECK(value.value == original_value);
}

TEST_CASE_TEMPLATE("saver_success restores only after normal exits",
                   T,
                   both_assignable,
                   copy_assignable_only,
                   move_assignable_only,
                   throwing_copy_assignable,
                   throwing_move_assignable) {
  T value{original_value};

  {
    nstd::saver_success<T&> saver{value};
    value.value = changed_value;
  }
  CHECK(value.value == original_value);

  CHECK_THROWS([&]() {
    nstd::saver_success<T&> saver{value};
    value.value = changed_value;
    throw std::runtime_error{"error"};
  }());
  CHECK(value.value == changed_value);
}

TEST_CASE("state_saver local macros select the requested exit policy") {
  int value = original_value;

  {
    SAVER_EXIT(value);
    value = changed_value;
  }
  CHECK(value == original_value);

  CHECK_THROWS([&]() {
    SAVER_FAIL(value);
    value = changed_value;
    throw std::runtime_error{"error"};
  }());
  CHECK(value == original_value);

  {
    SAVER_SUCCESS(value);
    value = changed_value;
  }
  CHECK(value == original_value);
}

TEST_CASE("state_saver block macros select the requested exit policy") {
  int value = original_value;

  WITH_SAVER_EXIT(value) {
    value = changed_value;
  }
  CHECK(value == original_value);

  WITH_SAVER_FAIL(value) {
    value = changed_value;
  }
  CHECK(value == changed_value);

  value = original_value;
  CHECK_THROWS([&]() {
    WITH_SAVER_FAIL(value) {
      value = changed_value;
      throw std::runtime_error{"error"};
    }
  }());
  CHECK(value == original_value);

  WITH_SAVER_SUCCESS(value) {
    value = changed_value;
  }
  CHECK(value == original_value);

  CHECK_THROWS([&]() {
    WITH_SAVER_SUCCESS(value) {
      value = changed_value;
      throw std::runtime_error{"error"};
    }
  }());
  CHECK(value == changed_value);
}

TEST_CASE("dismiss disables automatic restore") {
  int value = original_value;

  {
    nstd::saver_exit<int&> saver{value};
    value = changed_value;
    saver.dismiss();
  }
  CHECK(value == changed_value);

  value = original_value;
  CHECK_THROWS([&]() {
    nstd::saver_fail<int&> saver{value};
    value = changed_value;
    saver.dismiss();
    throw std::runtime_error{"error"};
  }());
  CHECK(value == changed_value);

  value = original_value;
  {
    nstd::saver_success<int&> saver{value};
    value = changed_value;
    saver.dismiss();
  }
  CHECK(value == changed_value);
}

TEST_CASE("restore keeps the guard active until dismiss") {
  int value = original_value;

  {
    nstd::saver_exit<int&> saver{value};
    value = changed_value;
    saver.restore();
    CHECK(value == original_value);
    value = changed_value;
    saver.restore();
    CHECK(value == original_value);
    value = changed_value;
  }
  CHECK(value == original_value);

  {
    nstd::saver_exit<int&> saver{value};
    value = changed_value;
    saver.dismiss();
    saver.restore();
    CHECK(value == original_value);
    value = changed_value;
  }
  CHECK(value == changed_value);
}

TEST_CASE("throwing explicit restore follows the active exit policy") {
  throwing_copy_assignable value{original_value};

  CHECK_THROWS([&]() {
    nstd::saver_exit<throwing_copy_assignable&> saver{value};
    value.value = changed_value;
    saver.restore();
  }());
  CHECK(value.value == original_value);

  value.value = original_value;
  CHECK_THROWS([&]() {
    nstd::saver_fail<throwing_copy_assignable&> saver{value};
    value.value = changed_value;
    saver.restore();
  }());
  CHECK(value.value == original_value);

  value.value = original_value;
  CHECK_THROWS([&]() {
    nstd::saver_success<throwing_copy_assignable&> saver{value};
    value.value = changed_value;
    saver.restore();
  }());
  CHECK(value.value == changed_value);
}

TEST_CASE("restore participates only for copy-assignable values") {
  static_assert(has_restore<nstd::saver_exit<both_assignable&>>::value,
                "restore should be available for copy-assignable values.");
  static_assert(!has_restore<nstd::saver_exit<move_assignable_only&>>::value,
                "restore should be unavailable for move-only assignable values.");
}

TEST_CASE("state_saver exception specifications match the selected expressions") {
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<both_assignable&>>::value,
                "nothrow move assignment should make automatic restore noexcept.");
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<copy_assignable_only&>>::value,
                "nothrow lvalue-only assignment should make automatic restore noexcept.");
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<move_assignable_only&>>::value,
                "nothrow rvalue-only assignment should make automatic restore noexcept.");
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<throwing_copy_assignable&>>::value,
                "default restore should use the noexcept rvalue assignment.");
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<throwing_move_assignable&>>::value,
                "default restore should fall back to the noexcept lvalue assignment.");
  static_assert(!std::is_nothrow_destructible<nstd::saver_exit<both_throwing_assignable&>>::value,
                "automatic restore may throw when both assignment forms may throw.");

  static_assert(has_nothrow_restore<nstd::saver_exit<both_assignable&>>::value,
                "restore() should be available and noexcept for nothrow lvalue assignment.");
  static_assert(has_restore<nstd::saver_exit<throwing_copy_assignable&>>::value,
                "restore() should be available for throwing lvalue assignment.");
  static_assert(!has_nothrow_restore<nstd::saver_exit<throwing_copy_assignable&>>::value,
                "restore() should reflect a throwing lvalue assignment.");
  static_assert(!has_restore<nstd::saver_exit<move_assignable_only&>>::value,
                "restore() should be unavailable without lvalue assignment.");
}

TEST_CASE("state_saver checks the actual lvalue snapshot construction") {
  static_assert(std::is_nothrow_constructible<lvalue_snapshot, lvalue_snapshot&>::value,
                "snapshot construction from the guarded lvalue should be noexcept.");
  static_assert(!std::is_nothrow_copy_constructible<lvalue_snapshot>::value,
                "the standard copy trait checks construction from const T& instead.");
  static_assert(std::is_nothrow_constructible<nstd::saver_exit<lvalue_snapshot&>, lvalue_snapshot&>::value,
                "state_saver should use the actual lvalue construction expression.");

  lvalue_snapshot value{original_value};
  {
    nstd::saver_exit<lvalue_snapshot&> saver{value};
    value.value = changed_value;
  }
  CHECK(value.value == original_value);
}

TEST_CASE("snapshot construction bypasses initializer-list constructors") {
  using T = initializer_list_snapshot;
  static_assert(std::is_nothrow_constructible<nstd::saver_exit<T>, T&>::value,
                "snapshot construction must agree with its noexcept trait.");
  T value(original_value);
  {
    nstd::saver_exit<T> saver(value);
    value.value = changed_value;
    saver.restore();
    CHECK(value.value == original_value);
    value.value = changed_value;
  }
  CHECK(value.value == original_value);
  {
    nstd::saver_success<T> saver(value);
    value.value = changed_value;
  }
  CHECK(value.value == original_value);
  CHECK_THROWS_AS([&] {
    nstd::saver_fail<T> saver(value);
    value.value = changed_value;
    throw std::runtime_error{"scope failure"};
  }(), const std::runtime_error&);
  CHECK(value.value == original_value);
}

TEST_CASE("default restore prefers lvalue assignment when both choices may throw") {
  both_throwing_assignable::copy_assign_count = 0;
  both_throwing_assignable::move_assign_count = 0;

  both_throwing_assignable value{original_value};
  {
    nstd::saver_exit<both_throwing_assignable&> saver{value};
    value.value = changed_value;
  }

  CHECK(value.value == original_value);
  CHECK(both_throwing_assignable::copy_assign_count == 1);
  CHECK(both_throwing_assignable::move_assign_count == 0);
}

TEST_CASE("throwing automatic restore propagates on a normal scope exit") {
  always_throwing_assignable exit_value{original_value};
  CHECK_THROWS_AS([&]() {
    nstd::saver_exit<always_throwing_assignable&> saver{exit_value};
    exit_value.value = changed_value;
  }(), const std::runtime_error&);
  CHECK(exit_value.value == changed_value);

  always_throwing_assignable success_value{original_value};
  CHECK_THROWS_AS([&]() {
    nstd::saver_success<always_throwing_assignable&> saver{success_value};
    success_value.value = changed_value;
  }(), const std::runtime_error&);
  CHECK(success_value.value == changed_value);
}

TEST_CASE("guards created during unwinding detect only newer exceptions") {
  int same_fail = original_value;
  int same_success = original_value;
  int nested_fail = original_value;
  int nested_success = original_value;

  CHECK_THROWS_AS([&]() {
    unwinding_probe probe(same_fail, same_success, nested_fail, nested_success);
    throw std::runtime_error{"outer failure"};
  }(), const std::runtime_error&);

  CHECK(same_fail == changed_value);
  CHECK(same_success == original_value);
  CHECK(nested_fail == original_value);
  CHECK(nested_success == changed_value);
}
