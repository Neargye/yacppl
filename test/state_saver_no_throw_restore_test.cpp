// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define STATE_SAVER_NO_THROW_RESTORE

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <state_saver.hpp>

#include <type_traits>
#include <utility>

TEST_CASE("state_saver no-throw restore mode keeps restore operations noexcept") {
  static_assert(std::is_nothrow_destructible<nstd::saver_exit<int&>>::value,
                "saver_exit<int&> should be nothrow destructible in no-throw restore mode.");

  int value = 1;
  nstd::saver_exit<int&> saver{value};
  value = 2;

  static_assert(noexcept(saver.restore()),
                "restore() should be noexcept in no-throw restore mode for nothrow assignable objects.");

  saver.restore();
  REQUIRE(value == 1);
}
