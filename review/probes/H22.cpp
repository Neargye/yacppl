// probe: H22 nstd::Trivial instantiates std::is_trivial, which C++26 deprecates (P3247R2)
// where: gcc-latest clang-libcxx macos-appleclang msvc-x64
// std: c++20 c++2c
// expect: build-ok
// expect c++2c: build-fail "deprecated"
// meaning: build-fail "deprecated" at c++2c = nstd::Trivial breaks -Werror builds with a C++26 standard library (H22 confirmed); build-ok at c++2c = this standard library does not deprecate is_trivial yet (not reproduced); "std not supported" = the compiler has no C++26 mode.
#include <concepts.hpp>

template <typename T, typename = nstd::Trivial<T>>
int trivial_only(T) {
  return 0;
}

int main() {
  return trivial_only(1);
}
