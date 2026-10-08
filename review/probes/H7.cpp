// probe: H7 MSVC fallback of ATTR_MAYBE_UNUSED (warning(suppress)) on a parameter of a multi-line function
// where: msvc-x64
// std: c++14 c++17
// expect c++14: build-fail "4100"
// expect c++17: build-ok
// meaning: C4100 in c++14 confirms H7 (suppress covers only the next line); build-ok refutes it.
#include <attributes.hpp>

static int pick(ATTR_MAYBE_UNUSED int unused,
                int value) {
  int result = value;
  result += 1;
  return result;
}

int main() {
  return pick(0, -1);
}
