// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <concepts.hpp>

#include <iostream>
#include <string>

template <typename T>
nstd::Integral<T> next_value(T value) {
  return static_cast<T>(value + 1);
}

template <typename T>
nstd::FloatingPoint<T> midpoint(T lhs, T rhs) {
  return (lhs + rhs) / T{2};
}

template <typename T>
nstd::Pointer<T> select_pointer(T ptr) {
  return ptr;
}

template <typename T, typename = nstd::Object<T>>
void reset_object(T& object) {
  object = T{};
}

template <typename T, typename = nstd::RValue<T>>
void consume_mutable_rvalue(T&& value) {
  std::cout << value << '\n';
}

int main() {
  int value = 41;
  int* ptr = &value;

  const int next = next_value(value);
  const double middle = midpoint(1.0, 3.0);
  const bool has_value = select_pointer(ptr) != nullptr;
  reset_object(value);

  std::cout << next << ' ' << middle << ' ' << has_value << ' ' << value << '\n';
  consume_mutable_rvalue(std::string{"concepts"});
  return 0;
}
