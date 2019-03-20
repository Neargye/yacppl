# Yet Another C++ Library

```text
__     __      _____ _____  _____  _
\ \   / //\   / ____|  __ \|  __ \| |
 \ \_/ //  \ | |    | |__) | |__) | |
  \   // /\ \| |    |  ___/|  ___/| |
   | |/ ____ \ |____| |    | |    | |____
   |_/_/    \_\_____|_|    |_|    |______
```

Branch | Linux/OSX | Windows | License | Codacy
-------|-----------|---------|---------|-------
master |[![Build Status](https://travis-ci.org/Neargye/yacppl.svg?branch=master)](https://travis-ci.org/Neargye/yacppl)|[![Build status](https://ci.appveyor.com/api/projects/status/7dmf3ri22cxfoaxm/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/yacppl/branch/master)|[![License](https://img.shields.io/github/license/Neargye/yacppl.svg)](LICENSE)|[![Codacy Badge](https://api.codacy.com/project/badge/Grade/70dfd3b3bb14434a8abffd1bb9fb593c)](https://www.codacy.com/app/Neargye/yacppl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/yacppl&amp;utm_campaign=Badge_Grade)

## [Nameof](https://github.com/Neargye/nameof) - Used to obtain the simple name of a variable, type, member, function, macros. Before, you had to use string literals to refer to definitions, which is brittle when renaming code elements because tools do not know to check these string literals.

```cpp
// Name of variable
NAMEOF(somevar) -> "somevar"
// Name of member
NAMEOF(person.address.zip_code) -> "zip_code"
// Name of function
NAMEOF(SomeMethod<int, float>) -> "SomeMethod"
NAMEOF_FULL(SomeMethod<int, float>) -> "SomeMethod4<int, float>"
// Name of enum
NAMEOF(SomeEnum::RED) -> "RED"
auto e = SomeEnum::RED;
NAMEOF_ENUM(e) -> "RED"
// Name of type
NAMEOF_TYPE(SomeEnum::RED) -> "SomeEnum"
NAMEOF_TYPE_T(int) -> "int"
// Name of macros
NAMEOF(__LINE__) -> "__LINE__"
```

## [SemVer](https://github.com/Neargye/semver) - C++ library compare and manipulate versions are available as extensions to the <major>.<minor>.<patch>-<prerelease>.<prereleaseversion> format complying with Semantic Versioning 2.0.0.

```cpp
constexpr semver::Version v(1, 2, 3, Version::PreReleaseType::kReleaseCandidate, 4);
const std::string s = v.ToString(); // "1.2.3-rc.4"
```

## [Scope Guard](https://github.com/Neargye/scope_guard) - Scope Guard & Defer C++.

```cpp
std::fstream file("test.txt");
SCOPE_EXIT{ file.close(); }; // File closes when exit the enclosing scope or errors occure.
```

## [State Saver](https://github.com/Neargye/state_saver) - Saves the origin variable value and restores, undoes any changes that could occure to the object.

```cpp
void Foo(A& a)
  STATE_SAVER_EXIT(a);
  ... // Alter state from non-const functions.
  ... // Other code.
  // Original state automatically restored on scope exit.
}
```

## [Unused](include/unused.hpp) - Macros with varying number of arguments to avoid "unused variable" warnings

```cpp
auto a = 0;
UNUSED(a); // No warning: unused variable 'a'.
```

## [Attributes](include/attributes.hpp) - Cross-compiler C++98/C++11/C++14/C++17/C++20 attributes

* ATTR_NORETURN - indicates that the function does not return.

```cpp
ATTR_NORETURN void foo() { std::terminate(); }
```

* ATTR_DEPRECATED - indicates that the use of the name or entity declared with this attribute is allowed, but discouraged for some reason.

```cpp
ATTR_DEPRECATED void foo() {}
```

* ATTR_FALLTHROUGH - indicates that the fall through from the previous case label is intentional and should not be diagnosed by a compiler that warns on fall-through.

```cpp
switch (expression) {
  case 1:
    ATTR_FALLTHROUGH
  default:
    break;
}
```

* ATTR_NODISCARD - encourages the compiler to issue a warning if the return value is discarded.

```cpp
ATTR_NODISCARD std::unique_ptr<int> foo() { return std::make_unique<int>(42); }
```

* ATTR_MAYBE_UNUSED - suppresses compiler warnings on unused entities, if any.

```cpp
ATTR_MAYBE_UNUSED int a = foo();
```

## [NSTD](include/nstd.hpp) - STD like stuff

* remove_all_p - removes all pointer from the given type.

```cpp
static_assert(std::is_same<nstd::remove_all_p<int**>::type, int>::value, "");
static_assert(std::is_same<nstd::remove_all_p_t<int* const* volatile *>, int>::value, "what?");
```

* remove_all_cvrp - removes all const, volatile, reference, pointer specifiers from the given type.

```cpp
const volatile int* const* volatile**** i;
static_assert(std::is_same<nstd::remove_all_cvrpe<decltype(i)>::type, int>::value, "");

const auto& r = &i;
static_assert(std::is_same<nstd::remove_all_cvrpe_t<decltype(r)>, int>::value, "");
```

* remove_all_cvrpe - removes all const, volatile, reference, pointer, array extents specifiers from the given type.

```cpp
const volatile int* const* volatile**** i[10];
static_assert(std::is_same<nstd::remove_all_cvrpe<decltype(i)>::type, int>::value, "");

const auto& r = &i;
static_assert(std::is_same<nstd::remove_all_cvrpe_t<decltype(r)>, int>::value, "");
```

* is_same_signed - checks if two types are the same signed/unsigned.

```cpp
static_assert(nstd::is_same_signed<unsigned int, unsigned short>::value, "");
static_assert(!nstd::is_same_signed_v<unsigned int, signed short>, "");
```

* move_assign_if_noexcept - returns an rvalue reference to arg, unless copy assign is a better option than move assign to provide at least a strong exception guarantee.

```cpp
a = nstd::move_assign_if_noexcept(b);
```

## Compiler compatibility

* GCC
* Clang
* MSVC

## Licensed under the [MIT License](LICENSE)
