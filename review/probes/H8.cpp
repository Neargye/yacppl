// probe: H8 ATTR_ALWAYS_INLINE (__forceinline without inline) defined in a header used by two TUs
// where: msvc-x64 msvc-x86 clangcl
// std: c++14 c++17
// with: H8_other.cpp
// expect: build-fail "LNK2005"
// meaning: LNK2005 (multiple definition) confirms H8; build-ok refutes it (__forceinline implies inline linkage).
#include "H8.hpp"

int other(int x);

int main() {
  return twice(1) + other(1) == 4 ? 0 : 1;
}
