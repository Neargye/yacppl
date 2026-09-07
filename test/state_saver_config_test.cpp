// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#if defined(STATE_SAVER_TEST_THROWING_HANDLER)
static void state_saver_throwing_catch_handler();
#  define STATE_SAVER_SUPPRESS_THROW_RESTORE
#  define STATE_SAVER_CATCH_HANDLER state_saver_throwing_catch_handler();
#elif defined(STATE_SAVER_TEST_SUPPRESS_THROW_RESTORE)
static void state_saver_catch_handler() noexcept;
#  define STATE_SAVER_SUPPRESS_THROW_RESTORE
#  define STATE_SAVER_CATCH_HANDLER state_saver_catch_handler();
#elif defined(STATE_SAVER_TEST_NO_THROW_RESTORE)
#  define STATE_SAVER_NO_THROW_RESTORE
#elif defined(STATE_SAVER_TEST_NO_THROW_CONSTRUCTIBLE)
#  define STATE_SAVER_NO_THROW_CONSTRUCTIBLE
#elif defined(STATE_SAVER_TEST_TERMINATE_RESTORE)
#  define STATE_SAVER_MAY_THROW_RESTORE
#elif defined(STATE_SAVER_TEST_FORCE_COPY_ASSIGNABLE)
#  define STATE_SAVER_FORCE_COPY_ASSIGNABLE
#elif defined(STATE_SAVER_TEST_FORCE_MOVE_ASSIGNABLE)
#  define STATE_SAVER_FORCE_MOVE_ASSIGNABLE
#else
#  error A state_saver test configuration must be selected.
#endif

#include <state_saver.hpp>

#if defined(STATE_SAVER_TEST_TERMINATE_RESTORE) || defined(STATE_SAVER_TEST_THROWING_HANDLER)

#include <cstdlib>
#include <exception>
#include <stdexcept>

namespace {

struct throwing_assign {
  int value;

  explicit throwing_assign(int value) : value{value} {}
  throwing_assign(const throwing_assign&) = default;
  throwing_assign(throwing_assign&&) = default;

  throwing_assign& operator=(const throwing_assign&) {
    throw std::runtime_error{"lvalue assignment"};
  }

  throwing_assign& operator=(throwing_assign&&) {
    throw std::runtime_error{"rvalue assignment"};
  }
};

[[noreturn]] void expected_terminate() noexcept {
  std::_Exit(EXIT_SUCCESS);
}

} // namespace

#  if defined(STATE_SAVER_TEST_TERMINATE_RESTORE)

static void unwind_with_throwing_restore() {
  throwing_assign value{1};
  nstd::saver_fail<throwing_assign&> saver{value};
  value.value = 2;
  throw std::runtime_error{"scope failure"};
}

int main() {
  std::set_terminate(expected_terminate);

  try {
    unwind_with_throwing_restore();
  } catch (...) {
  }

  return EXIT_FAILURE;
}

#  else

static void state_saver_throwing_catch_handler() {
  throw std::runtime_error{"catch handler"};
}

int main() {
  std::set_terminate(expected_terminate);

  throwing_assign value{1};
  nstd::saver_exit<throwing_assign&> saver{value};
  try {
    saver.restore();
  } catch (...) {
    std::_Exit(EXIT_FAILURE);
  }
  // Do not let the destructor turn an unexpected return into a successful termination test.
  std::_Exit(EXIT_FAILURE);
}

#  endif

#else

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <stdexcept>
#include <type_traits>
#include <utility>

#if defined(STATE_SAVER_TEST_SUPPRESS_THROW_RESTORE)

static int state_saver_catch_count = 0;

static void state_saver_catch_handler() noexcept {
  ++state_saver_catch_count;
}

namespace {

struct throwing_assign {
  static int assign_count;

  int value;

  explicit throwing_assign(int value) : value{value} {}
  throwing_assign(const throwing_assign&) = default;
  throwing_assign(throwing_assign&&) = default;

  throwing_assign& operator=(const throwing_assign&) {
    value = 100 + ++assign_count;
    throw std::runtime_error{"copy assignment"};
  }

  throwing_assign& operator=(throwing_assign&&) {
    value = 100 + ++assign_count;
    throw std::runtime_error{"move assignment"};
  }
};

int throwing_assign::assign_count = 0;

} // namespace

TEST_CASE("state_saver suppresses throwing restore operations") {
  state_saver_catch_count = 0;
  throwing_assign::assign_count = 0;
  throwing_assign value{1};

  {
    nstd::saver_exit<throwing_assign&> saver{value};
    value.value = 2;
  }
  CHECK(state_saver_catch_count == 1);
  CHECK(throwing_assign::assign_count == 1);
  CHECK(value.value == 101);

  {
    nstd::saver_exit<throwing_assign&> saver{value};
    saver.restore();
    saver.dismiss();
  }
  CHECK(state_saver_catch_count == 2);
  CHECK(throwing_assign::assign_count == 2);
  CHECK(value.value == 102);
}

TEST_CASE("state_saver keeps the guard active after a suppressed restore failure") {
  state_saver_catch_count = 0;
  throwing_assign::assign_count = 0;
  throwing_assign value{1};

  {
    nstd::saver_exit<throwing_assign&> saver{value};
    saver.restore();
  }

  CHECK(state_saver_catch_count == 2);
  CHECK(throwing_assign::assign_count == 2);
  CHECK(value.value == 102);
}

#elif defined(STATE_SAVER_TEST_NO_THROW_RESTORE)

namespace {

struct throwing_copy_nothrow_move {
  int value;

  explicit throwing_copy_nothrow_move(int value) : value{value} {}
  throwing_copy_nothrow_move(const throwing_copy_nothrow_move&) = default;
  throwing_copy_nothrow_move(throwing_copy_nothrow_move&&) = default;

  throwing_copy_nothrow_move& operator=(const throwing_copy_nothrow_move& other) noexcept(false) {
    value = other.value;
    return *this;
  }

  throwing_copy_nothrow_move& operator=(throwing_copy_nothrow_move&& other) noexcept {
    value = other.value;
    return *this;
  }
};

struct nothrow_lvalue_throwing_move {
  int value;

  explicit nothrow_lvalue_throwing_move(int value) : value{value} {}
  nothrow_lvalue_throwing_move(const nothrow_lvalue_throwing_move&) = default;
  nothrow_lvalue_throwing_move(nothrow_lvalue_throwing_move&&) = default;

  nothrow_lvalue_throwing_move& operator=(const nothrow_lvalue_throwing_move& other) noexcept {
    value = other.value;
    return *this;
  }

  nothrow_lvalue_throwing_move& operator=(nothrow_lvalue_throwing_move&& other) noexcept(false) {
    value = other.value;
    return *this;
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

} // namespace

TEST_CASE("state_saver no-throw mode keeps restore operations noexcept") {
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<int&>>::value,
                "saver_exit<int&> should be nothrow destructible.");
  static_assert(!has_restore<nstd::saver_exit<throwing_copy_nothrow_move&>>::value,
                "restore() should be unavailable when lvalue assignment may throw.");
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<throwing_copy_nothrow_move&>>::value,
                "no-throw mode should accept the selected noexcept rvalue assignment.");
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<nothrow_lvalue_throwing_move&>>::value,
                "no-throw mode should fall back to the noexcept lvalue assignment.");
  static_assert(has_restore<nstd::saver_exit<nothrow_lvalue_throwing_move&>>::value,
                "restore() should be available for noexcept lvalue assignment.");

  int value = 1;
  nstd::saver_exit<int&> saver{value};
  value = 2;

  static_assert(noexcept(saver.restore()), "restore() should be noexcept.");
  saver.restore();
  CHECK(value == 1);

  throwing_copy_nothrow_move movable{1};
  {
    nstd::saver_exit<throwing_copy_nothrow_move&> move_saver{movable};
    movable.value = 2;
  }
  CHECK(movable.value == 1);

  nothrow_lvalue_throwing_move copyable{1};
  {
    nstd::saver_exit<nothrow_lvalue_throwing_move&> copy_saver{copyable};
    copyable.value = 2;
  }
  CHECK(copyable.value == 1);
}

#elif defined(STATE_SAVER_TEST_NO_THROW_CONSTRUCTIBLE)

namespace {

struct lvalue_snapshot {
  int value;

  explicit lvalue_snapshot(int value) : value{value} {}
  lvalue_snapshot(lvalue_snapshot& other) noexcept : value{other.value} {}
  lvalue_snapshot(const lvalue_snapshot& other) noexcept(false) : value{other.value} {}
  lvalue_snapshot(lvalue_snapshot&&) noexcept = default;
  lvalue_snapshot& operator=(const lvalue_snapshot&) noexcept = default;
  lvalue_snapshot& operator=(lvalue_snapshot&&) noexcept = default;
};

} // namespace

TEST_CASE("state_saver no-throw construction mode checks the actual snapshot expression") {
  static_assert(std::is_nothrow_constructible<lvalue_snapshot, lvalue_snapshot&>::value,
                "snapshot construction from the guarded lvalue should be noexcept.");
  static_assert(!std::is_nothrow_copy_constructible<lvalue_snapshot>::value,
                "the standard copy trait checks construction from const T&.");
  static_assert(std::is_nothrow_constructible<nstd::saver_exit<lvalue_snapshot&>, lvalue_snapshot&>::value,
                "no-throw construction mode should accept the actual noexcept expression.");

  lvalue_snapshot value{1};
  {
    nstd::saver_exit<lvalue_snapshot&> saver{value};
    value.value = 2;
  }
  CHECK(value.value == 1);
}

#else

namespace {

struct assignment_counter {
  static int copy_assign_count;
  static int move_assign_count;

  int value;

  explicit assignment_counter(int value) : value{value} {}
  assignment_counter(const assignment_counter&) = default;
  assignment_counter(assignment_counter&&) = default;

  assignment_counter& operator=(const assignment_counter& other) noexcept {
    ++copy_assign_count;
    value = other.value;
    return *this;
  }

  assignment_counter& operator=(assignment_counter&& other) noexcept {
    ++move_assign_count;
    value = other.value;
    return *this;
  }
};

struct throwing_lvalue_nothrow_move {
  throwing_lvalue_nothrow_move() = default;
  throwing_lvalue_nothrow_move(const throwing_lvalue_nothrow_move&) = default;
  throwing_lvalue_nothrow_move(throwing_lvalue_nothrow_move&&) = default;
  throwing_lvalue_nothrow_move& operator=(const throwing_lvalue_nothrow_move&) noexcept(false) {
    return *this;
  }
  throwing_lvalue_nothrow_move& operator=(throwing_lvalue_nothrow_move&&) noexcept {
    return *this;
  }
};

struct nothrow_lvalue_throwing_move {
  nothrow_lvalue_throwing_move() = default;
  nothrow_lvalue_throwing_move(const nothrow_lvalue_throwing_move&) = default;
  nothrow_lvalue_throwing_move(nothrow_lvalue_throwing_move&&) = default;
  nothrow_lvalue_throwing_move& operator=(const nothrow_lvalue_throwing_move&) noexcept {
    return *this;
  }
  nothrow_lvalue_throwing_move& operator=(nothrow_lvalue_throwing_move&&) noexcept(false) {
    return *this;
  }
};

int assignment_counter::copy_assign_count = 0;
int assignment_counter::move_assign_count = 0;

} // namespace

TEST_CASE("state_saver forced assignment mode uses the requested operator") {
  assignment_counter::copy_assign_count = 0;
  assignment_counter::move_assign_count = 0;

  assignment_counter value{1};
  {
    nstd::saver_exit<assignment_counter&> saver{value};
    value.value = 2;
  }

  CHECK(value.value == 1);

#  if defined(STATE_SAVER_TEST_FORCE_COPY_ASSIGNABLE)
  static_assert(!std::is_nothrow_destructible<nstd::saver_exit<throwing_lvalue_nothrow_move&>>::value,
                "force-copy mode should expose throwing lvalue assignment.");
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<nothrow_lvalue_throwing_move&>>::value,
                "force-copy mode should expose noexcept lvalue assignment.");
  CHECK(assignment_counter::copy_assign_count == 1);
  CHECK(assignment_counter::move_assign_count == 0);
#  else
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<throwing_lvalue_nothrow_move&>>::value,
                "force-move mode should expose noexcept rvalue assignment.");
  static_assert(!std::is_nothrow_destructible<nstd::saver_exit<nothrow_lvalue_throwing_move&>>::value,
                "force-move mode should expose throwing rvalue assignment.");
  CHECK(assignment_counter::copy_assign_count == 0);
  CHECK(assignment_counter::move_assign_count == 1);
#  endif
}

#endif

#endif
