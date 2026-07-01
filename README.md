# Yet Another C++ Library

## A collection of small C++ utilities

* [state_saver](include/state_saver.hpp) - Saves an object's current value and restores it on scope exit, failure, or success.

* [attributes](include/attributes.hpp) - Attributes for C++98 and later.

* [UNUSED](include/unused.hpp) - Macro with varying number of arguments to avoid "unused variable" warnings, with no effect on binary size, for C++11 and later.

* [concepts](include/concepts.hpp) - Implementations concepts-like, for C++11 and later.

* [type_traits](include/type_traits.hpp) - Implementations of some type traits, for C++11 and later.

* [byte](include/byte.hpp) - Is a distinct type that implements the concept of byte as specified in the C++ language definition, with no aliasing to `char`, for C++17 and later.

* [utility](include/utility.hpp) - implementations of some utility functions, for C++17 and later.

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

## Licensed under the [MIT License](LICENSE)
