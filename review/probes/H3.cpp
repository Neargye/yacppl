// probe: H3 ATTR_ASSUME becomes a statement-only attribute in C++23 and is an expression before
// where: gcc14 clang20-libcxx
// std: c++17 c++23
// expect: build-ok
// expect c++23: build-fail
// meaning: build-fail in c++23 confirms H3; build-ok means the macro is still usable inside an expression.
#include <attributes.hpp>

static int deref(int* p) {
  return (ATTR_ASSUME(p != nullptr), *p);
}

int main() {
  int value = 0;
  return deref(&value);
}
