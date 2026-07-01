// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <type_traits.hpp>

#include <type_traits>

namespace {

struct copy_assignable {
  copy_assignable& operator=(const copy_assignable&) {
    return *this;
  }
};

struct not_copy_assignable {
  not_copy_assignable& operator=(const not_copy_assignable&) = delete;
};

struct class_type {};

struct no_value_member {};

enum enum_type : unsigned short {
  enum_value = 0
};

struct explicit_int {
  explicit explicit_int(int) {}
};

struct throwing_int {
  throwing_int(int) noexcept(false) {}
};

struct nothrow_int {
  nothrow_int(int) noexcept {}
};

template <typename T>
using copy_assign_t = decltype(std::declval<T&>() = std::declval<const T&>());

template <typename T>
using value_type_t = typename T::value_type;

} // namespace

TEST_CASE("type alias helpers match standard type transformations") {
  static_assert(std::is_same<nstd::bool_constant<true>, std::true_type>::value, "bool_constant<true> must match true_type.");
  static_assert(std::is_same<nstd::enable_if_t<true, int>, int>::value, "enable_if_t must expose T.");
  static_assert(std::is_same<nstd::conditional_t<true, int, long>, int>::value, "conditional_t true branch must match.");
  static_assert(std::is_same<nstd::conditional_t<false, int, long>, long>::value, "conditional_t false branch must match.");
  static_assert(std::is_same<nstd::decay_t<int&>, int>::value, "decay_t must remove references.");
  static_assert(std::is_same<nstd::remove_const_t<const int>, int>::value, "remove_const_t must remove const.");
  static_assert(std::is_same<nstd::remove_volatile_t<volatile int>, int>::value, "remove_volatile_t must remove volatile.");
  static_assert(std::is_same<nstd::remove_cv_t<const volatile int>, int>::value, "remove_cv_t must remove cv.");
  static_assert(std::is_same<nstd::remove_pointer_t<int*>, int>::value, "remove_pointer_t must remove one pointer.");
  static_assert(std::is_same<nstd::remove_reference_t<int&>, int>::value, "remove_reference_t must remove one reference.");
  static_assert(std::is_same<nstd::remove_extent_t<int[2]>, int>::value, "remove_extent_t must remove one array extent.");
  static_assert(std::is_same<nstd::remove_all_extents_t<int[2][3]>, int>::value, "remove_all_extents_t must remove all array extents.");
  static_assert(std::is_same<nstd::add_const_t<int>, const int>::value, "add_const_t must add const.");
  static_assert(std::is_same<nstd::add_volatile_t<int>, volatile int>::value, "add_volatile_t must add volatile.");
  static_assert(std::is_same<nstd::add_cv_t<int>, const volatile int>::value, "add_cv_t must add cv.");
  static_assert(std::is_same<nstd::add_pointer_t<int>, int*>::value, "add_pointer_t must add pointer.");
  static_assert(std::is_same<nstd::add_lvalue_reference_t<int>, int&>::value, "add_lvalue_reference_t must add lvalue reference.");
  static_assert(std::is_same<nstd::add_rvalue_reference_t<int>, int&&>::value, "add_rvalue_reference_t must add rvalue reference.");
  static_assert(std::is_same<nstd::make_signed_t<unsigned int>, int>::value, "make_signed_t must make signed type.");
  static_assert(std::is_same<nstd::make_unsigned_t<int>, unsigned int>::value, "make_unsigned_t must make unsigned type.");
  static_assert(std::is_same<nstd::underlying_type_t<enum_type>, unsigned short>::value, "underlying_type_t must expose enum underlying type.");
}

TEST_CASE("logical traits short-circuit and expose boolean results") {
  static_assert(nstd::conjunction<>::value, "empty conjunction must be true.");
  static_assert(nstd::conjunction<std::true_type, std::true_type>::value, "all true conjunction must be true.");
  static_assert(!nstd::conjunction<std::true_type, std::false_type>::value, "false conjunction member must make result false.");
  static_assert(!nstd::conjunction<std::false_type, no_value_member>::value, "conjunction must short-circuit after false.");
  static_assert(!nstd::disjunction<>::value, "empty disjunction must be false.");
  static_assert(nstd::disjunction<std::false_type, std::true_type>::value, "true disjunction member must make result true.");
  static_assert(!nstd::disjunction<std::false_type, std::false_type>::value, "all false disjunction must be false.");
  static_assert(nstd::disjunction<std::true_type, no_value_member>::value, "disjunction must short-circuit after true.");
  static_assert(nstd::negation<std::false_type>::value, "negation false must be true.");
  static_assert(!nstd::negation<std::true_type>::value, "negation true must be false.");

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
  static_assert(nstd::conjunction_v<std::true_type, std::true_type>, "conjunction_v must expose conjunction::value.");
  static_assert(nstd::disjunction_v<std::false_type, std::true_type>, "disjunction_v must expose disjunction::value.");
  static_assert(nstd::negation_v<std::false_type>, "negation_v must expose negation::value.");
#endif
}

TEST_CASE("detection idiom reports valid and invalid expressions") {
  static_assert(std::is_same<nstd::void_t<int, long>, void>::value, "void_t must expose void.");
  static_assert(nstd::is_detected<copy_assign_t, copy_assignable>::value, "copy assignment must be detected.");
  static_assert(!nstd::is_detected<copy_assign_t, not_copy_assignable>::value, "deleted copy assignment must not be detected.");
  static_assert(std::is_same<nstd::detected_t<copy_assign_t, copy_assignable>, copy_assignable&>::value, "detected_t must expose expression type.");
  static_assert(std::is_same<nstd::detected_or_t<int, value_type_t, class_type>, int>::value, "detected_or_t must expose default when detection fails.");
  static_assert(nstd::is_detected_exact<copy_assignable&, copy_assign_t, copy_assignable>::value, "is_detected_exact must match exact type.");
  static_assert(nstd::is_detected_convertible<copy_assignable&, copy_assign_t, copy_assignable>::value, "is_detected_convertible must match convertible type.");
}

TEST_CASE("custom removal traits remove all requested qualifiers") {
  static_assert(std::is_same<nstd::identity_t<int>, int>::value, "identity_t must expose T.");
  static_assert(std::is_same<nstd::type_identity<int>::type, int>::value, "type_identity must expose T.");
  static_assert(std::is_same<nstd::type_identity_t<int>, int>::value, "type_identity_t must expose T.");
  static_assert(std::is_same<nstd::remove_ptr_t<int*>, int>::value, "remove_ptr_t must remove one pointer.");
  static_assert(std::is_same<nstd::remove_ref_t<int&>, int>::value, "remove_ref_t must remove one reference.");
  static_assert(std::is_same<nstd::remove_cv_ref_t<const volatile int&>, int>::value, "remove_cv_ref_t must remove cv and reference.");
  static_assert(std::is_same<nstd::remove_cvref_t<const volatile int&>, int>::value, "remove_cvref_t must remove cv and reference.");
  static_assert(std::is_same<nstd::remove_all_ptr_t<int***>, int>::value, "remove_all_ptr_t must remove all pointer levels.");
  static_assert(std::is_same<nstd::remove_all_ptr_t<int* const* volatile>, int>::value, "remove_all_ptr_t must remove cv-qualified pointer levels.");
  static_assert(std::is_same<nstd::remove_all_cv_ref_ptr_t<const int* const volatile&>, int>::value, "remove_all_cv_ref_ptr_t must remove cv/ref/pointers.");
  static_assert(std::is_same<nstd::remove_all_cv_ref_ptr_ext_t<const int* const volatile (&)[2]>, int>::value, "remove_all_cv_ref_ptr_ext_t must remove cv/ref/pointers/arrays.");
}

TEST_CASE("same signedness only matches actual signed or unsigned categories") {
  static_assert(nstd::is_same_signedness<int, long>::value, "signed integral types must have same signedness.");
  static_assert(nstd::is_same_signedness<unsigned int, unsigned long>::value, "unsigned integral types must have same signedness.");
  static_assert(!nstd::is_same_signedness<int, unsigned int>::value, "signed and unsigned integral types must differ.");
  static_assert(nstd::is_same_signedness<double, int>::value, "floating point and signed integral types are both signed.");
  static_assert(!nstd::is_same_signedness<class_type, class_type>::value, "non-arithmetic types must not compare as same signedness.");
}

TEST_CASE("nothrow convertibility follows noexcept conversion semantics") {
  static_assert(nstd::is_nothrow_convertible<int, long>::value, "int must be nothrow convertible to long.");
  static_assert(!nstd::is_nothrow_convertible<int, explicit_int>::value, "explicit conversion constructor is not implicit convertible.");
  static_assert(!nstd::is_nothrow_convertible<int, throwing_int>::value, "throwing conversion must not be nothrow convertible.");
  static_assert(nstd::is_nothrow_convertible<int, nothrow_int>::value, "nothrow conversion must be nothrow convertible.");
  static_assert(nstd::is_nothrow_convertible<void, void>::value, "void must be nothrow convertible to void.");
  static_assert(!nstd::is_nothrow_convertible<int, void>::value, "int must not be convertible to void.");
  static_assert(!nstd::is_nothrow_convertible<void, int>::value, "void must not be convertible to int.");
  static_assert(!nstd::is_nothrow_convertible<int, int&>::value, "int must not be convertible to int&.");
  static_assert(nstd::is_nothrow_convertible<int&, int&>::value, "int& must be nothrow convertible to int&.");
  static_assert(nstd::is_nothrow_convertible<int&, const int&>::value, "int& must be nothrow convertible to const int&.");

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304L
  static_assert(nstd::is_same_signedness_v<int, long>, "is_same_signedness_v must expose is_same_signedness::value.");
  static_assert(nstd::is_nothrow_convertible_v<int, long>, "is_nothrow_convertible_v must expose is_nothrow_convertible::value.");
#endif
}
