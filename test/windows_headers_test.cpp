// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Daniil Goncharov <neargye@gmail.com>.

#include <windows.h>

#include <byte.hpp>
#include <utility.hpp>

int main() {
  const int original = 7;
  nstd::byte bytes[sizeof(original)];
  nstd::to_bytes(bytes, original);
  return nstd::from_bytes<int>(bytes) == original && nstd::in_range<int>(7u) ? 0 : 1;
}
