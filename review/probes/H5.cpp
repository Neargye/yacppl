// probe: H5 ATTR_ASSUME changes from an expression to a statement-only attribute in the C++23 [[assume]] branch
// where: gcc-latest clang-libcxx macos-appleclang msvc-x64 clangcl
// std: c++17 c++23
// expect: build-ok
// expect c++23: build-fail
// meaning: build-ok at c++17 and build-fail only at c++23 = the [[assume]] branch was taken and ATTR_ASSUME is no longer usable as an expression (H5 confirmed); build-ok at c++23 = the branch is not taken on this compiler (GCC < 14, Clang < 19, MSVC, ClangCL) or the role is preserved (H5 not reproduced there).
#include <attributes.hpp>

int positive(int x) {
  return (ATTR_ASSUME(x > 0), x);
}

int main() {
  return positive(1) - 1;
}
