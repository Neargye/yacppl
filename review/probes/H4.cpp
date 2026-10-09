// probe: H4 ATTR_ASSUME expands to the no-op static_cast<void>(0) on GCC although GCC supports __attribute__((assume)) in every mode
// where: gcc-latest
// std: c++17 c++20 c++23
// expect: build-fail "ATTR_ASSUME is a no-op"
// expect c++23: build-ok
// meaning: build-fail with the message = the macro expanded to static_cast<void>(0) while the compiler supports assume (H4 confirmed for this std); at c++23 GCC >= 14 reports __cplusplus = 202302L and should take the [[assume]] branch, so build-fail at c++23 means the job runs GCC 13 or the branch is still not taken (H4 also holds at c++23 there).
#include <attributes.hpp>

#define NSTD_PROBE_STR2(x) #x
#define NSTD_PROBE_STR(x) NSTD_PROBE_STR2(x)

constexpr bool same(const char* a, const char* b) {
  return *a == *b && (*a == '\0' || same(a + 1, b + 1));
}

#if defined(__GNUC__) && !defined(__clang__) && defined(__has_attribute)
#  if __has_attribute(assume)
static_assert(!same(NSTD_PROBE_STR(ATTR_ASSUME(true)), "static_cast<void>(0)"), "ATTR_ASSUME is a no-op although the compiler supports assume");
#  endif
#endif

int main() {
  return 0;
}
