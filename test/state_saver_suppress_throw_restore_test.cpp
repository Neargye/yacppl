// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <stdexcept>

static void state_saver_catch_handler() noexcept;

#define STATE_SAVER_SUPPRESS_THROW_RESTORE
#define STATE_SAVER_CATCH_HANDLER state_saver_catch_handler();

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <state_saver.hpp>

static int state_saver_catch_count = 0;

static void state_saver_catch_handler() noexcept {
  ++state_saver_catch_count;
}

class throwing_assign {
 public:
  int i = 0;

  explicit throwing_assign(int value) : i{value} {}
  throwing_assign(const throwing_assign&) = default;
  throwing_assign(throwing_assign&&) = default;

  throwing_assign& operator=(const throwing_assign&) {
    throw std::runtime_error{"copy assignment"};
  }

  throwing_assign& operator=(throwing_assign&&) {
    throw std::runtime_error{"move assignment"};
  }
};

TEST_CASE("state_saver suppresses throwing destructor restore and calls handler") {
  state_saver_catch_count = 0;
  throwing_assign value{1};

  {
    nstd::saver_exit<throwing_assign&> saver{value};
    value.i = 2;
  }

  REQUIRE(state_saver_catch_count == 1);
  REQUIRE(value.i == 2);
}

TEST_CASE("state_saver suppresses throwing explicit restore and calls handler") {
  state_saver_catch_count = 0;
  throwing_assign value{1};

  {
    nstd::saver_exit<throwing_assign&> saver{value};
    value.i = 2;

    saver.restore();
    saver.dismiss();
  }

  REQUIRE(state_saver_catch_count == 1);
  REQUIRE(value.i == 2);
}
