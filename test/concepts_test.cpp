// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <concepts.hpp>

#include <string>
#include <type_traits>

namespace {

enum enum_type {
  enum_value
};

struct class_type {};

union union_type {
  int value;
};

struct non_default_constructible {
  non_default_constructible() = delete;
  explicit non_default_constructible(int) {}
};

struct int_constructible {
  explicit int_constructible(int) {}
};

struct throwing_int_constructible {
  explicit throwing_int_constructible(int) noexcept(false) {}
};

struct non_copy_constructible {
  non_copy_constructible() = default;
  non_copy_constructible(const non_copy_constructible&) = delete;
};

struct non_move_constructible {
  non_move_constructible() = default;
  non_move_constructible(non_move_constructible&&) = delete;
};

struct non_copy_assignable {
  non_copy_assignable& operator=(const non_copy_assignable&) = delete;
};

struct non_move_assignable {
  non_move_assignable& operator=(non_move_assignable&&) = delete;
};

struct throwing_assignable {
  throwing_assignable& operator=(const throwing_assignable&) noexcept(false) {
    return *this;
  }
};

struct non_destructible {
  ~non_destructible() = delete;
};

struct throwing_destructible {
  ~throwing_destructible() noexcept(false) {}
};

template <template <typename> class Concept, typename T>
struct is_concept_well_formed {
  template <typename U, typename = Concept<U>>
  static std::true_type test(int);

  template <typename>
  static std::false_type test(...);

  static const bool value = decltype(test<T>(0))::value;
};

template <template <typename, typename> class Concept, typename T, typename U>
struct is_binary_concept_well_formed {
  template <typename X, typename Y, typename = Concept<X, Y>>
  static std::true_type test(int);

  template <typename, typename>
  static std::false_type test(...);

  static const bool value = decltype(test<T, U>(0))::value;
};

template <template <typename, typename...> class Concept, typename T, typename... Args>
struct is_variadic_concept_well_formed {
  template <typename X, typename = Concept<X, Args...>>
  static std::true_type test(int);

  template <typename>
  static std::false_type test(...);

  static const bool value = decltype(test<T>(0))::value;
};

template <typename T, typename Enable = nstd::EnableIf<std::is_integral<T>::value>>
T add_one(T value) {
  return static_cast<T>(value + 1);
}

template <typename T>
nstd::Integral<T> integral_identity(T value) {
  return value;
}

template <typename T, typename = nstd::RValue<T>>
bool accepts_mutable_rvalue(T&&) {
  return true;
}

} // namespace

TEST_CASE("concept aliases participate in SFINAE") {
  CHECK(add_one(41) == 42);
  CHECK(integral_identity(7) == 7);
  CHECK(accepts_mutable_rvalue(std::string{"text"}));
}

TEST_CASE("concept aliases preserve the constrained type") {
  static_assert(std::is_same<nstd::EnableIf<true, int>, int>::value, "EnableIf must expose the requested type.");
  static_assert(std::is_same<nstd::TypeConcept<int, true>, int>::value, "TypeConcept must expose T.");
  static_assert(std::is_same<nstd::RValue<int>, int&&>::value, "RValue must expose an rvalue reference.");
  static_assert(std::is_same<nstd::LValue<int&>, int&>::value, "LValue must preserve an lvalue reference.");
  static_assert(std::is_same<nstd::Const<const int>, const int>::value, "Const must preserve const-qualified types.");
  static_assert(std::is_same<nstd::NotConst<int>, int>::value, "NotConst must preserve non-const types.");
}

TEST_CASE("type category concepts accept and reject expected types") {
  static_assert(is_concept_well_formed<nstd::RValue, int>::value, "int models RValue when used as a forwarding-reference deduction result.");
  static_assert(!is_concept_well_formed<nstd::RValue, int&>::value, "int& does not model RValue.");
  static_assert(!is_concept_well_formed<nstd::RValue, const int>::value, "const int does not model mutable RValue.");
  static_assert(is_concept_well_formed<nstd::LValue, int&>::value, "int& models LValue.");
  static_assert(!is_concept_well_formed<nstd::LValue, int>::value, "int does not model LValue.");
  static_assert(is_concept_well_formed<nstd::Reference, int&>::value, "int& models Reference.");
  static_assert(is_concept_well_formed<nstd::Reference, int&&>::value, "int&& models Reference.");
  static_assert(!is_concept_well_formed<nstd::Reference, int>::value, "int does not model Reference.");

  static_assert(is_concept_well_formed<nstd::Const, const int>::value, "const int models Const.");
  static_assert(!is_concept_well_formed<nstd::Const, int>::value, "int does not model Const.");
  static_assert(is_concept_well_formed<nstd::NotConst, int>::value, "int models NotConst.");
  static_assert(!is_concept_well_formed<nstd::NotConst, const int>::value, "const int does not model NotConst.");
  static_assert(is_concept_well_formed<nstd::Volatile, volatile int>::value, "volatile int models Volatile.");
  static_assert(!is_concept_well_formed<nstd::Volatile, int>::value, "int does not model Volatile.");
  static_assert(is_concept_well_formed<nstd::NotVolatile, int>::value, "int models NotVolatile.");
  static_assert(!is_concept_well_formed<nstd::NotVolatile, volatile int>::value, "volatile int does not model NotVolatile.");

  static_assert(is_concept_well_formed<nstd::Void, void>::value, "void models Void.");
  static_assert(!is_concept_well_formed<nstd::Void, int>::value, "int does not model Void.");
  static_assert(is_concept_well_formed<nstd::NullPointer, decltype(nullptr)>::value, "nullptr_t models NullPointer.");
  static_assert(!is_concept_well_formed<nstd::NullPointer, int*>::value, "int* does not model NullPointer.");
  static_assert(is_concept_well_formed<nstd::Arithmetic, int>::value, "int models Arithmetic.");
  static_assert(is_concept_well_formed<nstd::Arithmetic, double>::value, "double models Arithmetic.");
  static_assert(!is_concept_well_formed<nstd::Arithmetic, class_type>::value, "class_type does not model Arithmetic.");
  static_assert(is_concept_well_formed<nstd::Integral, int>::value, "int models Integral.");
  static_assert(!is_concept_well_formed<nstd::Integral, double>::value, "double does not model Integral.");
  static_assert(is_concept_well_formed<nstd::SignedIntegral, int>::value, "int models SignedIntegral.");
  static_assert(!is_concept_well_formed<nstd::SignedIntegral, unsigned>::value, "unsigned does not model SignedIntegral.");
  static_assert(is_concept_well_formed<nstd::UnsignedIntegral, unsigned>::value, "unsigned models UnsignedIntegral.");
  static_assert(!is_concept_well_formed<nstd::UnsignedIntegral, int>::value, "int does not model UnsignedIntegral.");
  static_assert(is_concept_well_formed<nstd::FloatingPoint, double>::value, "double models FloatingPoint.");
  static_assert(!is_concept_well_formed<nstd::FloatingPoint, int>::value, "int does not model FloatingPoint.");
  static_assert(is_concept_well_formed<nstd::Enum, enum_type>::value, "enum_type models Enum.");
  static_assert(!is_concept_well_formed<nstd::Enum, int>::value, "int does not model Enum.");
  static_assert(is_concept_well_formed<nstd::Pointer, int*>::value, "int* models Pointer.");
  static_assert(!is_concept_well_formed<nstd::Pointer, int>::value, "int does not model Pointer.");
  static_assert(is_concept_well_formed<nstd::MemberPointer, int class_type::*>::value, "int class_type::* models MemberPointer.");
  static_assert(!is_concept_well_formed<nstd::MemberPointer, int*>::value, "int* does not model MemberPointer.");
  static_assert(is_concept_well_formed<nstd::Array, int[2]>::value, "int[2] models Array.");
  static_assert(!is_concept_well_formed<nstd::Array, int*>::value, "int* does not model Array.");
  static_assert(is_concept_well_formed<nstd::Function, void()>::value, "void() models Function.");
  static_assert(!is_concept_well_formed<nstd::Function, void (*)()>::value, "function pointer does not model Function.");
  static_assert(is_concept_well_formed<nstd::Object, class_type>::value, "class_type models Object.");
  static_assert(is_concept_well_formed<nstd::Object, int[2]>::value, "int[2] models Object.");
  static_assert(!is_concept_well_formed<nstd::Object, void>::value, "void does not model Object.");
  static_assert(is_concept_well_formed<nstd::Scalar, int*>::value, "int* models Scalar.");
  static_assert(!is_concept_well_formed<nstd::Scalar, class_type>::value, "class_type does not model Scalar.");
  static_assert(is_concept_well_formed<nstd::Class, class_type>::value, "class_type models Class.");
  static_assert(!is_concept_well_formed<nstd::Class, int>::value, "int does not model Class.");
  static_assert(is_concept_well_formed<nstd::Union, union_type>::value, "union_type models Union.");
  static_assert(!is_concept_well_formed<nstd::Union, class_type>::value, "class_type does not model Union.");
}

TEST_CASE("object capability concepts accept and reject expected types") {
  static_assert(is_concept_well_formed<nstd::Trivial, int>::value, "int models Trivial.");
  static_assert(is_concept_well_formed<nstd::TriviallyCopyable, int>::value, "int models TriviallyCopyable.");

  static_assert(is_variadic_concept_well_formed<nstd::Constructible, int>::value, "int models Constructible<>.");
  static_assert(is_variadic_concept_well_formed<nstd::Constructible, int_constructible, int>::value, "int_constructible models Constructible<int>.");
  static_assert(!is_variadic_concept_well_formed<nstd::Constructible, int_constructible>::value, "int_constructible does not model Constructible<>.");
  static_assert(is_variadic_concept_well_formed<nstd::Constructible, int&, int&>::value, "int& models Constructible<int&>.");
  static_assert(!is_variadic_concept_well_formed<nstd::Constructible, int&, int>::value, "int& does not model Constructible<int>.");
  static_assert(is_variadic_concept_well_formed<nstd::NothrowConstructible, int, int>::value, "int models NothrowConstructible<int>.");
  static_assert(!is_variadic_concept_well_formed<nstd::NothrowConstructible, throwing_int_constructible, int>::value, "throwing_int_constructible does not model NothrowConstructible<int>.");
  static_assert(is_concept_well_formed<nstd::DefaultConstructible, int>::value, "int models DefaultConstructible.");
  static_assert(!is_concept_well_formed<nstd::DefaultConstructible, int&>::value, "int& does not model DefaultConstructible.");
  static_assert(!is_concept_well_formed<nstd::DefaultConstructible, non_default_constructible>::value, "non_default_constructible does not model DefaultConstructible.");
  static_assert(is_concept_well_formed<nstd::CopyConstructible, int>::value, "int models CopyConstructible.");
  static_assert(is_concept_well_formed<nstd::CopyConstructible, int&>::value, "int& models CopyConstructible.");
  static_assert(!is_concept_well_formed<nstd::CopyConstructible, non_copy_constructible>::value, "non_copy_constructible does not model CopyConstructible.");
  static_assert(is_concept_well_formed<nstd::MoveConstructible, int>::value, "int models MoveConstructible.");
  static_assert(!is_concept_well_formed<nstd::MoveConstructible, non_move_constructible>::value, "non_move_constructible does not model MoveConstructible.");
  static_assert(is_concept_well_formed<nstd::Destructible, int>::value, "int models Destructible.");
  static_assert(!is_concept_well_formed<nstd::Destructible, non_destructible>::value, "non_destructible does not model Destructible.");
  static_assert(is_concept_well_formed<nstd::NothrowDestructible, int>::value, "int models NothrowDestructible.");
  static_assert(!is_concept_well_formed<nstd::NothrowDestructible, throwing_destructible>::value, "throwing_destructible does not model NothrowDestructible.");
  static_assert(is_concept_well_formed<nstd::CopyAssignable, int>::value, "int models CopyAssignable.");
  static_assert(!is_concept_well_formed<nstd::CopyAssignable, non_copy_assignable>::value, "non_copy_assignable does not model CopyAssignable.");
  static_assert(is_concept_well_formed<nstd::MoveAssignable, int>::value, "int models MoveAssignable.");
  static_assert(!is_concept_well_formed<nstd::MoveAssignable, non_move_assignable>::value, "non_move_assignable does not model MoveAssignable.");
}

TEST_CASE("binary concept aliases accept and reject expected relations") {
  static_assert(is_binary_concept_well_formed<nstd::Assignable, int&, int>::value, "int& models Assignable<int>.");
  static_assert(!is_binary_concept_well_formed<nstd::Assignable, const int&, int>::value, "const int& does not model Assignable<int>.");
  static_assert(is_binary_concept_well_formed<nstd::NothrowAssignable, int&, int>::value, "int& models NothrowAssignable<int>.");
  static_assert(!is_binary_concept_well_formed<nstd::NothrowAssignable, throwing_assignable&, const throwing_assignable&>::value, "throwing_assignable& does not model NothrowAssignable<const throwing_assignable&>.");
  static_assert(is_binary_concept_well_formed<nstd::Same, int, int>::value, "int models Same<int>.");
  static_assert(!is_binary_concept_well_formed<nstd::Same, int&, int>::value, "int& does not model Same<int>.");
  static_assert(!is_binary_concept_well_formed<nstd::Same, const int, int>::value, "const int does not model Same<int>.");
  static_assert(is_binary_concept_well_formed<nstd::ConvertibleTo, int, double>::value, "int models ConvertibleTo<double>.");
  static_assert(is_binary_concept_well_formed<nstd::ConvertibleTo, const char*, std::string>::value, "const char* models ConvertibleTo<string>.");
  static_assert(!is_binary_concept_well_formed<nstd::ConvertibleTo, class_type, int>::value, "class_type does not model ConvertibleTo<int>.");
}
