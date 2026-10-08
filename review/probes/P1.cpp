// probe: P1 which ATTR_ASSUME branch is selected (boundary of empirical problem 1)
// where: gcc14 clang20-libcxx msvc-x64 clangcl
// std: c++17 c++23
// expect: run-ok
// meaning: prints the language version and the expansion; static_cast<void>(0) means a no-op.
#include <attributes.hpp>

#include <cstdio>

#define PROBE_STR_(...) #__VA_ARGS__
#define PROBE_STR(...) PROBE_STR_(__VA_ARGS__)

#if defined(_MSVC_LANG)
#  define PROBE_LANG _MSVC_LANG
#else
#  define PROBE_LANG __cplusplus
#endif

int main() {
  std::printf("%ld %s\n", static_cast<long>(PROBE_LANG), PROBE_STR(ATTR_ASSUME(x)));
  return 0;
}
