# Yet Another C++ Library

A collection of standalone C++ utilities.

* [state_saver](include/state_saver.hpp) - scope guards for C++11 and later that restore a saved value on exit, failure, or success.
* [attributes](include/attributes.hpp) - portable attribute macros for C++98 and later.
* [unused](include/unused.hpp) - helpers for suppressing unused-variable warnings in C++11 and later.
* [concepts](include/concepts.hpp) - SFINAE-friendly aliases for C++11 and later.
* [type_traits](include/type_traits.hpp) - additional type traits for C++11 and later.
* [byte](include/byte.hpp) - a distinct byte type and byte-copy helpers for C++17 and later.
* [utility](include/utility.hpp) - general utility helpers for C++17 and later.

A combined [nstd_example.cpp](example/nstd_example.cpp) demonstrates several modules together.

## attributes

`attributes` provides portability macros for common compiler and standard attributes. Unsupported attributes use a safe fallback.

### Declaration attributes

* `ATTR_NORETURN` - marks a function that does not return.
* `ATTR_ALWAYS_INLINE` - requests function inlining.
* `ATTR_DEPRECATED("reason")` - marks a declaration as deprecated.
* `ATTR_NODISCARD` - asks the compiler to warn when a return value is discarded.
* `ATTR_NODISCARD_MSG("reason")` - same as `ATTR_NODISCARD`, with a diagnostic reason when supported.
* `ATTR_MAYBE_UNUSED` - suppresses unused warnings on declarations.
* `ATTR_TRIVIAL_ABI` - requests Clang's `trivial_abi` for eligible classes. Put it after `class` or `struct`, for example `struct ATTR_TRIVIAL_ABI type`.
* `ATTR_NO_UNIQUE_ADDRESS` - allows a non-static data member to share an address when supported.

### Statement and expression helpers

* `ATTR_ASSUME(expr)` - tells the optimizer that `expr` is true. Do not use it for validation or pass expressions with side effects.
* `ATTR_FALLTHROUGH` - marks an intentional `switch` fallthrough.
* `ATTR_LIKELY(expr)` / `ATTR_UNLIKELY(expr)` - mark likely and unlikely branch conditions.

See [attributes_example.cpp](example/attributes_example.cpp) for a complete example.

## unused

`unused` provides local-scope helpers for intentionally unused variables and parameters.

* `nstd::unused(args...)` - evaluates its arguments and binds them by reference.
* `NSTD_UNUSED(args...)` - macro helper for one or more variables and parameters. Its arguments are not evaluated.

Use `ATTR_MAYBE_UNUSED` from [attributes](include/attributes.hpp) for declaration-level suppression and `NSTD_UNUSED` for local expression-level suppression.

See [unused_example.cpp](example/unused_example.cpp) for a complete example.

## byte

`byte` provides a C++17 `std::byte`-style type in the `nstd` namespace. It is not arithmetic and does not alias `char`.

### Operations

* `nstd::byte` - scoped enum backed by `unsigned char`.
* `nstd::to_byte(value)` - converts an integral value to `nstd::byte`.
* `nstd::to_integer<T>(byte)` - converts a byte to an integral type.
* Bitwise and shift operators are supported. Shift counts must be non-negative integral values other than `bool` and smaller than the bit width of `unsigned int`.
* `nstd::to_bytes` and `nstd::from_bytes` copy trusted object representations of trivially copyable types; they do not validate serialized or untrusted input.
* The return-by-value `nstd::from_bytes<T>` also requires a default constructor and a trivial copy or move constructor; the overload that writes to an existing object does not.
* Counted copies support overlap and treat zero count as a no-op, including null pointers. Non-empty copies require valid pointers and sufficiently large buffers; overflowing counts are rejected without copying.

See [byte_example.cpp](example/byte_example.cpp) for a complete example.

## concepts

`concepts` provides C++11 SFINAE aliases, not C++20 `concept` declarations. An alias resolves to the constrained type when its predicate is true.

```cpp
#include <concepts.hpp>

template <typename T>
nstd::Integral<T> next_value(T value) {
  return static_cast<T>(value + 1);
}
```

### Building blocks

* `nstd::EnableIf<C, T>` - exposes `T` when `C` is true.
* `nstd::TypeConcept<T, C>` - exposes `T` when `C` is true.

### Type categories

* `nstd::RValue<T>` / `nstd::LValue<T>` - forwarding-reference deduction result category. `RValue` rejects const rvalues.
* `nstd::Reference<T>` - lvalue or rvalue reference.
* `nstd::Const<T>` / `nstd::NotConst<T>` - top-level const qualification after reference removal.
* `nstd::Volatile<T>` / `nstd::NotVolatile<T>` - top-level volatile qualification after reference removal.
* `nstd::Void<T>` and `nstd::NullPointer<T>`.
* `nstd::Arithmetic<T>`, `nstd::Integral<T>`, `nstd::SignedIntegral<T>`, `nstd::UnsignedIntegral<T>`, `nstd::FloatingPoint<T>`.
* `nstd::Enum<T>`, `nstd::Pointer<T>`, `nstd::MemberPointer<T>`, `nstd::Array<T>`, `nstd::Function<T>`.
* `nstd::Object<T>`, `nstd::Scalar<T>`, `nstd::Class<T>`, `nstd::Union<T>`.
* `nstd::Trivial<T>` and `nstd::TriviallyCopyable<T>`.

### Object capabilities and relations

* `nstd::Constructible<T, Args...>` and `nstd::NothrowConstructible<T, Args...>`.
* `nstd::DefaultConstructible<T>`, `nstd::CopyConstructible<T>`, `nstd::MoveConstructible<T>`.
* `nstd::Destructible<T>` and `nstd::NothrowDestructible<T>`.
* `nstd::Assignable<T, U>` and `nstd::NothrowAssignable<T, U>`.
* `nstd::CopyAssignable<T>` and `nstd::MoveAssignable<T>`.
* `nstd::Same<T, U>` - exactly the same type.
* `nstd::ConvertibleTo<T, U>` - `T` is convertible to `U`.

See [concepts_example.cpp](example/concepts_example.cpp) for a complete example.

## type_traits

`type_traits` provides C++11-compatible helpers for type transformation, detection, and a few missing standard-library traits.

Variable-template `_v` forms are available when supported by the selected language standard.

### Alias helpers

* `nstd::bool_constant<B>`, `nstd::enable_if_t<C, T>`, `nstd::conditional_t<C, T, F>`.
* `nstd::decay_t<T>`, `nstd::remove_const_t<T>`, `nstd::remove_volatile_t<T>`, `nstd::remove_cv_t<T>`.
* `nstd::remove_pointer_t<T>`, `nstd::remove_reference_t<T>`, `nstd::remove_extent_t<T>`, `nstd::remove_all_extents_t<T>`.
* `nstd::add_const_t<T>`, `nstd::add_volatile_t<T>`, `nstd::add_cv_t<T>`.
* `nstd::add_pointer_t<T>`, `nstd::add_lvalue_reference_t<T>`, `nstd::add_rvalue_reference_t<T>`.
* `nstd::make_signed_t<T>` and `nstd::make_unsigned_t<T>`.
* `nstd::underlying_type_t<T>`.

### Detection idiom

* `nstd::void_t<T...>`.
* `nstd::is_detected<Op, Args...>` / `nstd::is_detected_v<Op, Args...>`.
* `nstd::detected_t<Op, Args...>`, `nstd::detected_or<Default, Op, Args...>`, and `nstd::detected_or_t<Default, Op, Args...>`.
* `nstd::is_detected_exact<Expected, Op, Args...>` / `nstd::is_detected_exact_v<Expected, Op, Args...>`.
* `nstd::is_detected_convertible<To, Op, Args...>` / `nstd::is_detected_convertible_v<To, Op, Args...>`.

### Transformations and predicates

* `nstd::identity<T>` / `nstd::identity_t<T>` and `nstd::type_identity<T>` / `nstd::type_identity_t<T>`.
* `nstd::remove_ptr<T>` / `nstd::remove_ptr_t<T>`.
* `nstd::remove_ref<T>` / `nstd::remove_ref_t<T>`.
* `nstd::remove_cv_ref<T>` / `nstd::remove_cv_ref_t<T>` and `nstd::remove_cvref_t<T>`.
* `nstd::remove_all_ptr<T>` / `nstd::remove_all_ptr_t<T>`.
* `nstd::remove_all_cv_ref_ptr<T>` / `nstd::remove_all_cv_ref_ptr_t<T>`.
* `nstd::remove_all_cv_ref_ptr_ext<T>` / `nstd::remove_all_cv_ref_ptr_ext_t<T>`.
* `nstd::conjunction<T...>`, `nstd::disjunction<T...>`, and `nstd::negation<T>`.
* `nstd::is_same_signedness<T, U>` - true when both types have the same signedness.
* `nstd::is_nothrow_convertible<From, To>` - C++11-compatible backport of `std::is_nothrow_convertible`.

See [type_traits_example.cpp](example/type_traits_example.cpp) for a complete example.

## state_saver

`state_saver` restores a saved object value on exit, failure, or success.

```cpp
#include <state_saver.hpp>

void use_temporary_value(int& value) {
  SAVER_EXIT(value);

  value = 42;
  // value is restored when the scope exits, including exception unwinding.
}
```

### Guards

* `nstd::saver_exit<T>` / `SAVER_EXIT(x)` - restores on scope exit.
* `nstd::saver_fail<T>` / `SAVER_FAIL(x)` - restores when scope exits while a new exception is being unwound.
* `nstd::saver_success<T>` / `SAVER_SUCCESS(x)` - restores when scope exits without a new exception being unwound.
* `MAKE_SAVER_EXIT(name, x)`, `MAKE_SAVER_FAIL(name, x)`, and `MAKE_SAVER_SUCCESS(name, x)` create named guards.
* `WITH_SAVER_EXIT(x)`, `WITH_SAVER_FAIL(x)`, and `WITH_SAVER_SUCCESS(x)` create a guard for a simple block. Use a named guard if the block needs `break` or `continue`.

See [state_saver_exit_example.cpp](example/state_saver_exit_example.cpp), [state_saver_fail_example.cpp](example/state_saver_fail_example.cpp), and [state_saver_success_example.cpp](example/state_saver_success_example.cpp) for complete examples.

### Interface

* `dismiss()` disables automatic restore.
* `restore()` restores immediately. It does not dismiss the guard, so the destructor may restore again later unless `dismiss()` is called.

### Requirements

The object must be non-const, non-pointer, non-array, non-function, constructible from a non-const lvalue, assignable, and nothrow destructible. Automatic restore assigns from an rvalue if that expression is `noexcept` or lvalue assignment is unavailable; otherwise it assigns from an lvalue. `restore()` assigns from an lvalue.

### Configuration

Set configuration macros before including `<state_saver.hpp>` and use the same values in every translation unit. At most one restore policy may be defined:

* `STATE_SAVER_MAY_THROW_RESTORE` - restore may throw; this is the default.
* `STATE_SAVER_NO_THROW_RESTORE` - requires noexcept restore.
* `STATE_SAVER_SUPPRESS_THROW_RESTORE` - catches restore exceptions.

With the default policy, an automatic restore that throws during exception unwinding calls `std::terminate`. Use the no-throw policy for noexcept assignment or the suppress policy when this must be avoided.

Optional settings:

* `STATE_SAVER_CATCH_HANDLER` - handles suppressed restore exceptions and must not throw.
* `STATE_SAVER_NO_THROW_CONSTRUCTIBLE` - requires nothrow saved-value construction.
* `STATE_SAVER_FORCE_MOVE_ASSIGNABLE` - forces automatic assignment from an rvalue.
* `STATE_SAVER_FORCE_COPY_ASSIGNABLE` - forces automatic assignment from an lvalue.

## utility

`utility` provides C++17 forwarding, conversion, invocation, and comparison helpers.

### Forwarding and move helpers

* `nstd::move(x)` - move cast that rejects const objects.
* `nstd::move_if_noexcept(x)` - move-construction helper matching `std::move_if_noexcept`, with const-object rejection.
* `nstd::move_assign_if_noexcept(x)` - assignment-oriented equivalent that falls back to `const T&` when move assignment may throw and copy assignment is available.
* `nstd::forward<T>(x)` - forwarding helper matching `std::forward`.
* `nstd::forward_like<T>(x)` - C++23-style helper that applies `T`'s const/reference qualifiers to `x`.
* `nstd::unforward(x)` and `nstd::decay_copy(x)` - value-materialization helpers.

### Invocation and conversion helpers

* `nstd::bit_cast<To>(from)` - C++17 cast between same-size trivially copyable types. `To` must be non-cv, have a default constructor and a trivial copy or move constructor, and accept the source representation. Padding bytes need not be preserved.
* `nstd::invoke_each(f, args...)` - invokes `f` once for each argument.
* `nstd::apply_each(f, tuple_like)` - invokes `f` once for each element of a tuple-like object such as `std::tuple`, `std::pair`, or `std::array`.
* `nstd::to_underlying(e)` - C++23-style enum-to-underlying conversion.
* `nstd::cmp_equal`, `nstd::cmp_not_equal`, `nstd::cmp_less`, `nstd::cmp_greater`, `nstd::cmp_less_equal`, and `nstd::cmp_greater_equal` - C++20-style signed/unsigned integer comparisons without lossy casts.
* `nstd::in_range<T>(value)` - C++20-style signed/unsigned integer range check.
* `nstd::constexpr_for<Start, End, Inc>(f)` - increasing loop over same-type non-bool integral constants. `Inc` must be positive.

See [utility_example.cpp](example/utility_example.cpp) for a complete example.

## Licensed under the [MIT License](LICENSE)
