# Yet Another C++ Library

## A collection of small C++ utilities

* [state_saver](include/state_saver.hpp) - Saves an object's current value and restores it on scope exit, failure, or success.

* [attributes](include/attributes.hpp) - Attributes for C++98 and later.

* [UNUSED](include/unused.hpp) - Macro with varying number of arguments to avoid "unused variable" warnings, with no effect on binary size, for C++11 and later.

* [concepts](include/concepts.hpp) - SFINAE-friendly concept-like aliases for C++11 and later.

* [type_traits](include/type_traits.hpp) - Implementations of some type traits, for C++11 and later.

* [byte](include/byte.hpp) - Is a distinct type that implements the concept of byte as specified in the C++ language definition, with no aliasing to `char`, for C++17 and later.

* [utility](include/utility.hpp) - implementations of some utility functions, for C++17 and later.

## attributes

`attributes` provides small portability macros for common compiler and standard attributes. Unsupported attributes degrade to a safe fallback.

### Declaration attributes

* `ATTR_NORETURN` - marks a function that does not return.
* `ATTR_ALWAYS_INLINE` - strongly suggests that a function should be inlined.
* `ATTR_DEPRECATED("reason")` - marks a declaration as deprecated.
* `ATTR_NODISCARD` - asks the compiler to warn when a return value is discarded.
* `ATTR_NODISCARD_MSG("reason")` - same as `ATTR_NODISCARD`, with a reason on compilers that support C++20 `[[nodiscard("reason")]]`.
* `ATTR_MAYBE_UNUSED` - suppresses unused warnings on declarations.
* `ATTR_TRIVIAL_ABI` - requests Clang's `trivial_abi` calling convention for eligible class types; other compilers use a no-op fallback.
* `ATTR_NO_UNIQUE_ADDRESS` - marks a non-static data member that does not need a distinct address on compilers that support C++20 `[[no_unique_address]]`.

### Statement and expression helpers

* `ATTR_ASSUME(expr)` - optimizer assumption statement. On supporting compilers, behavior is undefined if `expr` is false; unsupported compilers use a no-op fallback that does not evaluate `expr`.
* `ATTR_FALLTHROUGH` - marks an intentional `switch` fallthrough.
* `ATTR_LIKELY(expr)` - expression helper for a likely branch condition.
* `ATTR_UNLIKELY(expr)` - expression helper for an unlikely branch condition.

See [attributes_example.cpp](example/attributes_example.cpp) for a complete example.

## UNUSED

`unused` provides local-scope helpers for intentionally unused variables and parameters.

* `nstd::unused(args...)` - function helper for one or more variables and parameters. It evaluates arguments like a normal function call and binds them by reference without copying.
* `NSTD_UNUSED(args...)` - macro helper for one or more variables and parameters. On non-MSVC compilers it checks arguments through an unevaluated `decltype` expression; on MSVC it uses the traditional `(void)` path. Do not pass expressions with side effects; use an explicit `(void)expr` when evaluating and discarding an expression is intended.

Use `ATTR_MAYBE_UNUSED` from [attributes](include/attributes.hpp) for declaration-level suppression and `NSTD_UNUSED` for local expression-level suppression.

See [unused_example.cpp](example/unused_example.cpp) for a complete example.

## byte

`byte` provides a C++17 `std::byte`-style distinct byte type in the `nstd` namespace. It is not an arithmetic type and does not alias `char`; use explicit conversion helpers when an integer value is needed.

### Operations

* `nstd::byte` - scoped enum backed by `unsigned char`.
* `nstd::to_byte(value)` - converts an integral value to `nstd::byte`.
* `nstd::to_integer<T>(byte)` - converts a byte to an integral type.
* Bitwise operators are supported: `~`, `|`, `&`, `^`, `<<`, `>>`, and their compound-assignment forms.
* `nstd::to_bytes(dst, value)` and `nstd::from_bytes<T>(src)` copy a single trivially copyable object through byte storage. The return-by-value `from_bytes<T>` form also requires `T` to be default constructible.
* `nstd::from_bytes(value, src)` copies into an existing trivially copyable object, including objects that are not default constructible.
* `nstd::to_bytes(dst, values, count)` and `nstd::from_bytes(dst, src, count)` copy arrays of trivially copyable objects. Bounded C-style arrays also have overloads that infer the element count.

See [byte_example.cpp](example/byte_example.cpp) for a complete example.

## concepts

`concepts` provides C++11-compatible type aliases that behave like lightweight constraints in SFINAE contexts. They are not C++20 `concept` declarations; each alias exposes the constrained type when the predicate is true and is ill-formed during substitution when the predicate is false.

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
* `nstd::is_detected<Op, Args...>` and, when variable templates are supported, `nstd::is_detected_v<Op, Args...>`.
* `nstd::detected_t<Op, Args...>`, `nstd::detected_or<Default, Op, Args...>`, and `nstd::detected_or_t<Default, Op, Args...>`.
* `nstd::is_detected_exact<Expected, Op, Args...>`.
* `nstd::is_detected_convertible<To, Op, Args...>`.

### Transformations and predicates

* `nstd::identity<T>` / `nstd::identity_t<T>` and `nstd::type_identity<T>` / `nstd::type_identity_t<T>`.
* `nstd::remove_ptr<T>` / `nstd::remove_ptr_t<T>`.
* `nstd::remove_ref<T>` / `nstd::remove_ref_t<T>`.
* `nstd::remove_cv_ref<T>` / `nstd::remove_cv_ref_t<T>` and `nstd::remove_cvref_t<T>`.
* `nstd::remove_all_ptr<T>` / `nstd::remove_all_ptr_t<T>`.
* `nstd::remove_all_cv_ref_ptr<T>` / `nstd::remove_all_cv_ref_ptr_t<T>`.
* `nstd::remove_all_cv_ref_ptr_ext<T>` / `nstd::remove_all_cv_ref_ptr_ext_t<T>`.
* `nstd::conjunction<T...>`, `nstd::disjunction<T...>`, and `nstd::negation<T>`.
* `nstd::is_same_signedness<T, U>` - true only when both types are signed or both types are unsigned.
* `nstd::is_nothrow_convertible<From, To>` - C++11-compatible backport of `std::is_nothrow_convertible`.

See [type_traits_example.cpp](example/type_traits_example.cpp) for a complete example.

## state_saver

`state_saver` saves a copy of an object and restores that value later. It is useful for temporarily changing configuration flags, counters, stream state, or any other assignable object whose original value must be restored reliably.

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
* `WITH_SAVER_EXIT(x)`, `WITH_SAVER_FAIL(x)`, and `WITH_SAVER_SUCCESS(x)` create a block-scoped guard before the block body.

See [state_saver_exit_example.cpp](example/state_saver_exit_example.cpp), [state_saver_fail_example.cpp](example/state_saver_fail_example.cpp), and [state_saver_success_example.cpp](example/state_saver_success_example.cpp) for complete examples.

```cpp
void run_with_saved_state(int& value) {
  WITH_SAVER_EXIT(value) {
    value = 7;
    // value is restored before control leaves this block.
  }
}
```

### Interface

* `dismiss()` disables automatic restore.
* `restore()` restores immediately. It does not dismiss the guard, so the destructor may restore again later unless `dismiss()` is called.

### Requirements

The saved object must be non-const, non-pointer, non-array, non-function, copy constructible, and assignable. Automatic restore chooses copy or move assignment from the configured/default assignment policy. `restore()` always requires copy assignment because it keeps the saved value available for a later automatic restore.

### Configuration

Only one restore policy may be defined:

* `STATE_SAVER_MAY_THROW_RESTORE` - default; restore may throw.
* `STATE_SAVER_NO_THROW_RESTORE` - require noexcept restore.
* `STATE_SAVER_SUPPRESS_THROW_RESTORE` - suppress restore exceptions.

Optional settings:

* `STATE_SAVER_CATCH_HANDLER` - handler used by `STATE_SAVER_SUPPRESS_THROW_RESTORE`.
* `STATE_SAVER_NO_THROW_CONSTRUCTIBLE` - require nothrow saved-value construction.
* `STATE_SAVER_FORCE_MOVE_ASSIGNABLE` - force destructor restore through move assignment.
* `STATE_SAVER_FORCE_COPY_ASSIGNABLE` - force destructor restore through copy assignment.

## utility

`utility` provides C++17 helpers for forwarding, safe move-selection, byte-preserving casts, and applying callables to argument packs or tuple-like values.

### Forwarding and move helpers

* `nstd::move(x)` - move cast that rejects const objects.
* `nstd::move_if_noexcept(x)` - move-construction helper matching `std::move_if_noexcept`, with const-object rejection.
* `nstd::move_assign_if_noexcept(x)` - assignment-oriented equivalent that falls back to `const T&` when move assignment may throw and copy assignment is available.
* `nstd::forward<T>(x)` - forwarding helper matching `std::forward`.
* `nstd::forward_like<T>(x)` - C++23-style helper that applies `T`'s cv-ref qualifiers to `x`.
* `nstd::unforward(x)` and `nstd::decay_copy(x)` - value-materialization helpers.

### Invocation and conversion helpers

* `nstd::bit_cast<To>(from)` - C++17 byte-preserving cast for same-size trivially copyable types. This portable implementation also requires `To` to be trivially default constructible.
* `nstd::invoke_each(f, args...)` - invokes `f` once for each argument.
* `nstd::apply_each(f, tuple_like)` - invokes `f` once for each element of a tuple-like object such as `std::tuple`, `std::pair`, or `std::array`.
* `nstd::to_underlying(e)` - C++23-style enum-to-underlying conversion.
* `nstd::cmp_equal`, `nstd::cmp_not_equal`, `nstd::cmp_less`, `nstd::cmp_greater`, `nstd::cmp_less_equal`, and `nstd::cmp_greater_equal` - C++20-style integer comparisons that handle mixed signedness without lossy casts.
* `nstd::in_range<T>(value)` - C++20-style check that an integer value is representable by `T`.
* `nstd::constexpr_for<Start, End, Inc>(f)` - compile-time increasing loop over integral constants. `Inc` must be positive.

See [utility_example.cpp](example/utility_example.cpp) for a complete example.

## Licensed under the [MIT License](LICENSE)
