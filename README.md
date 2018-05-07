# Yet Another C++ Macros Library

```text
__     __      _____ _____  _____  __  __ _
\ \   / //\   / ____|  __ \|  __ \|  \/  | |
 \ \_/ //  \ | |    | |__) | |__) | \  / | |
  \   // /\ \| |    |  ___/|  ___/| |\/| | |
   | |/ ____ \ |____| |    | |    | |  | | |____
   |_/_/    \_\_____|_|    |_|    |_|  |_|______|
```

Branch | Linux/OSX | Windows | License | Codacy
-------|-----------|---------|---------|-------
master |[![Build Status](https://travis-ci.org/Neargye/yacppml.svg?branch=master)](https://travis-ci.org/Neargye/yacppml)|[![Build status](https://ci.appveyor.com/api/projects/status/irdag2r6tsubtthx/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/yacppml/branch/master)|[![License](https://img.shields.io/github/license/Neargye/yacppml.svg)](LICENSE)|[![Codacy Badge](https://api.codacy.com/project/badge/Grade/70dfd3b3bb14434a8abffd1bb9fb593c)](https://www.codacy.com/app/Neargye/yacppml?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/yacppml&amp;utm_campaign=Badge_Grade)

Сollection of operators-like macros С++11:

* [nameof](https://github.com/Neargye/nameof) - Simple name of a variable, type, member, function, macros and etc.

```cpp
std::cout << NAMEOF(person.address.zip_code) << std::endl; // prints "zip_code"
```

* [scope_guard](https://github.com/Neargye/scope_guard) - Scope Guard & Defer C++.

```cpp
std::fstream file("test.txt");
DEFER{ file.close(); }; // File closes when exit the enclosing scope or errors occure.
```

* [state_save](https://github.com/Neargye/state_save) - Saves the origin variable value and restores on scope exit, undoes any changes that could occure to the object.

```cpp
void Foo(A& a)
  STATE_SAVER(a);
  ... // Alter state from non-const functions.
  ... // Other code.
  // Original state automatically restored.
}
```

Сollection of useful С++11 macros:

* [unused](include/unused.hpp) - Macros with varying number of arguments to avoid "unused variable" warnings.

```cpp
auto d = 0;
UNUSED(d); // No warning: unused variable 'd'.
```

## Licensed under the [MIT License](LICENSE)
