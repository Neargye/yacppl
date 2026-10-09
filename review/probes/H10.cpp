// probe: H10 ATTR_NODISCARD is empty (or SAL-only _Check_return_) on MSVC before /std:c++17, so discarding the result is not diagnosed
// where: msvc-x64 msvc-x86
// std: c++14 c++17
// expect c++14: build-ok
// expect c++17: build-fail "C4834"
// meaning: c++14 build-ok while c++17 fails with C4834 = the macro is a no-op in MSVC's default mode although MSVC accepts [[nodiscard]] there (H10 confirmed); c++14 build-fail with C4834 = refuted. ClangCL is not listed: it takes the __clang__ branch first.
#include <attributes.hpp>

ATTR_NODISCARD int compute() {
  return 42;
}

int main() {
  compute();
  return 0;
}
