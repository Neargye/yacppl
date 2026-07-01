// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <type_traits.hpp>

#include <iostream>
#include <type_traits>
#include <utility>

struct record {
  using value_type = int;
};

struct no_value_type {};

template <typename T>
using value_type_t = typename T::value_type;

template <typename T>
using copy_assign_t = decltype(std::declval<T&>() = std::declval<const T&>());

template <typename T, typename = nstd::enable_if_t<nstd::is_detected<value_type_t, T>::value>>
typename T::value_type default_value() {
  return typename T::value_type{};
}

int main() {
  static_assert(std::is_same<nstd::remove_all_cv_ref_ptr_ext_t<const int* const (&)[2]>, int>::value,
                "remove_all_cv_ref_ptr_ext_t must remove cv/ref/pointer/array wrappers.");
  static_assert(nstd::is_detected<value_type_t, record>::value, "record has value_type.");
  static_assert(!nstd::is_detected<value_type_t, no_value_type>::value, "no_value_type has no value_type.");
  static_assert(nstd::is_detected<copy_assign_t, record>::value, "record is copy assignable.");
  static_assert(nstd::conjunction<std::is_integral<int>, nstd::is_same_signedness<int, long>>::value,
                "int and long are signed integral types.");

  std::cout << default_value<record>() << '\n';
  return 0;
}
