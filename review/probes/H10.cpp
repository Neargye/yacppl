// probe: H10 nstd::Trivial uses std::is_trivial, deprecated in C++26
// where: gcc14 clang20-libcxx
// std: c++20 c++2c
// expect: build-ok
// expect c++2c: build-fail "deprecated"
// meaning: a deprecation error in c++2c confirms H10 for that standard library; build-ok means it is not deprecated there yet.
#include <concepts.hpp>

template <typename T>
nstd::Trivial<T> identity_value(T value) {
  return value;
}

int main() {
  return identity_value(0);
}
