// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <state_saver.hpp>

#if !defined(STATE_SAVER_FORCE_COPY_ASSIGNABLE) && !defined(STATE_SAVER_FORCE_MOVE_ASSIGNABLE)
#  error This test requires STATE_SAVER_FORCE_COPY_ASSIGNABLE or STATE_SAVER_FORCE_MOVE_ASSIGNABLE.
#endif

class assignment_counter {
 public:
  static int copy_assign_count;
  static int move_assign_count;

  int i = 0;

  explicit assignment_counter(int value) : i{value} {}
  assignment_counter(const assignment_counter&) = default;
  assignment_counter(assignment_counter&&) = default;

  assignment_counter& operator=(const assignment_counter& other) noexcept {
    ++copy_assign_count;
    i = other.i;
    return *this;
  }

  assignment_counter& operator=(assignment_counter&& other) noexcept {
    ++move_assign_count;
    i = other.i;
    return *this;
  }
};

int assignment_counter::copy_assign_count = 0;
int assignment_counter::move_assign_count = 0;

TEST_CASE("state_saver forced assignment mode uses the requested operator") {
  assignment_counter::copy_assign_count = 0;
  assignment_counter::move_assign_count = 0;

  assignment_counter value{1};

  {
    nstd::saver_exit<assignment_counter&> saver{value};
    value.i = 2;
  }

  REQUIRE(value.i == 1);

#if defined(STATE_SAVER_FORCE_COPY_ASSIGNABLE)
  REQUIRE(assignment_counter::copy_assign_count == 1);
  REQUIRE(assignment_counter::move_assign_count == 0);
#elif defined(STATE_SAVER_FORCE_MOVE_ASSIGNABLE)
  REQUIRE(assignment_counter::copy_assign_count == 0);
  REQUIRE(assignment_counter::move_assign_count == 1);
#endif
}
