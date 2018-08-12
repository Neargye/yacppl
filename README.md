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

## [Unused](include/unused.hpp) - Macros with varying number of arguments to avoid "unused variable" warnings

```cpp
auto a = 0;
UNUSED(a); // No warning: unused variable 'a'.
```

## [Cpp Attributes](include/cpp_attributes.hpp) - Cross-compiler C++ attributes

* CPP_ATTR_NORETURN - indicates that the function does not return.

```cpp
CPP_ATTR_NORETURN void foo() { std::terminate(); }
```

* CPP_ATTR_DEPRECATED - indicates that the use of the name or entity declared with this attribute is allowed, but discouraged for some reason.

```cpp
CPP_ATTR_DEPRECATED void foo() {}
```

* CPP_ATTR_FALLTHROUGH - indicates that the fall through from the previous case label is intentional and should not be diagnosed by a compiler that warns on fall-through.

```cpp
switch (expression) {
  case 1:
    CPP_ATTR_FALLTHROUGH
  default:
    break;
}
```

* CPP_ATTR_NODISCARD - encourages the compiler to issue a warning if the return value is discarded.

```cpp
CPP_ATTR_NODISCARD std::unique_ptr<int> foo() { return std::make_unique<int>(42); }
```

* CPP_ATTR_MAYBE_UNUSED - suppresses compiler warnings on unused entities, if any.

```cpp
CPP_ATTR_MAYBE_UNUSED int a = foo();
```

## Compiler compatibility

* GCC
* Clang
* MSVC

## Licensed under the [MIT License](LICENSE)
