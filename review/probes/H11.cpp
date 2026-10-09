// probe: H11 ATTR_MAYBE_UNUSED pre-C++17 MSVC fallback __pragma(warning(suppress : 4100 4101 4189)) covers only the next line
// where: msvc-x64 msvc-x86
// std: c++14 c++17
// expect c++14: build-fail "warning C41"
// expect c++17: build-ok
// meaning: build-fail with C4100/C4101/C4189 only at c++14 = the suppress fallback does not cover these placements (H11 confirmed); build-ok at c++14 = the fallback is sufficient for them (H11 refuted). c++17 uses [[maybe_unused]] and is the control. ClangCL is not listed: it takes the __clang__ branch first.
#include <attributes.hpp>

int two_line_params(int used,
                    ATTR_MAYBE_UNUSED int unused_second) {
  return used;
}

int main() {
  ATTR_MAYBE_UNUSED int same_line = 1;
  ATTR_MAYBE_UNUSED
  int next_line = 2;
  ATTR_MAYBE_UNUSED const int
      wrapped = 3;
  return two_line_params(0, 1);
}
