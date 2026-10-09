// probe: H23 _v traits and constexpr nstd::unused depend on language feature-test macros that the tests guard the same way
// where: msvc-x64 msvc-x86 clangcl macos-appleclang
// std: c++14 c++17 c++20
// expect: build-fail
// meaning: build-fail (conjunction_v / is_nothrow_convertible_v missing, or nstd::unused not constexpr) = the compiler supports the feature but does not define the feature-test macro, part of the API silently disappears (H23 confirmed for this job); build-ok = the macros are defined, H23 refuted for this job.
#include <type_traits.hpp>
#include <unused.hpp>

#include <type_traits>

static_assert(nstd::conjunction_v<std::true_type, std::true_type>, "conjunction_v must exist from C++14");
static_assert(nstd::is_nothrow_convertible_v<int, long>, "is_nothrow_convertible_v must exist from C++14");

constexpr int use(int x) {
  nstd::unused(x);
  return 0;
}

static_assert(use(1) == 0, "nstd::unused must be constexpr from C++14");

int main() {
  return 0;
}
