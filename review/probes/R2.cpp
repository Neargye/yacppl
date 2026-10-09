// probe: R2 uncaught_exceptions() branch selection: MSVC and ClangCL take std::uncaught_exceptions() already in C++14 (_MSC_VER >= 1900), libc++ takes it from C++17; the count must be right on every branch (profile risk 2)
// where: msvc-x64 msvc-x86 clangcl macos-appleclang clang-libcxx
// std: c++14 c++17 c++20
// expect: run-ok
// meaning: build-fail at c++14 on msvc/clangcl means std::uncaught_exceptions() is not available in the pre-C++17 MSVC branch; build-fail at c++17 on macos/libc++ means std::uncaught_exceptions() is unavailable for the default deployment target; run-fail means the count is wrong during unwinding. run-ok closes R2 for these jobs except old macOS deployment targets (-mmacosx-version-min cannot be set by a probe).
#include <state_saver.hpp>

#include <cstdio>

namespace {

int failures = 0;

void check(int expected, const char* where) {
  const int actual = nstd::detail::uncaught_exceptions();
  if (actual != expected) {
    std::printf("%s: expected %d, got %d\n", where, expected, actual);
    ++failures;
  }
}

struct check_on_unwind {
  int expected;
  ~check_on_unwind() {
    check(expected, "check_on_unwind");
  }
};

struct nested_thrower {
  ~nested_thrower() {
    try {
      check_on_unwind inner = {2};
      static_cast<void>(inner);
      throw 2;
    } catch (int) {
    }
  }
};

} // namespace

int main() {
  check(0, "start");
  try {
    check_on_unwind outer = {1};
    static_cast<void>(outer);
    throw 1;
  } catch (int) {
  }
  try {
    nested_thrower thrower;
    static_cast<void>(thrower);
    throw 1;
  } catch (int) {
  }
  int value = 0;
  try {
    SAVER_FAIL(value);
    value = 5;
    throw 1;
  } catch (int) {
  }
  if (value != 0) {
    std::printf("saver_fail did not restore on exception\n");
    ++failures;
  }
  {
    SAVER_SUCCESS(value);
    value = 6;
  }
  if (value != 0) {
    std::printf("saver_success did not restore on normal exit\n");
    ++failures;
  }
  std::printf(failures == 0 ? "ok\n" : "failed\n");
  return failures == 0 ? 0 : 1;
}
