// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Daniil Goncharov <neargye@gmail.com>.

#include "../include/state_saver.hpp"

struct value {
  value() = default;
  value(const value&) noexcept = default;
  value& operator=(const value&) noexcept = default;

#if defined(TEST_THROWING_DESTRUCTOR)
  ~value() noexcept(false) {}
#else
  ~value() noexcept = default;
#endif
};

int main() {
  value object;
  nstd::saver_exit<value> guard(object);
  guard.dismiss();
}
