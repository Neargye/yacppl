// probe: R1 pre-C++17 uncaught_exceptions() via __cxa_get_globals matches the real count during unwinding (profile risk 1)
// where: macos-appleclang clang20-libcxx gcc14 gcc-m32
// std: c++11 c++14
// expect: run-ok
// meaning: run-fail means the fixed offset reads the wrong field of __cxa_eh_globals for this ABI library.
#include <state_saver.hpp>

#include <cstdio>

static int failures = 0;

struct check_on_unwind {
  int expected;
  ~check_on_unwind() {
    const int actual = nstd::detail::uncaught_exceptions();
    if (actual != expected) {
      std::printf("expected %d, got %d\n", expected, actual);
      ++failures;
    }
  }
};

struct nested_thrower {
  ~nested_thrower() {
    try {
      check_on_unwind check = {2};
      throw 2;
    } catch (int) {
    }
  }
};

int main() {
  {
    check_on_unwind check = {0};
  }
  try {
    check_on_unwind check = {1};
    throw 1;
  } catch (int) {
  }
  try {
    nested_thrower thrower;
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
    SAVER_FAIL(value);
    value = 6;
  }
  if (value != 6) {
    std::printf("saver_fail restored without exception\n");
    ++failures;
  }

  std::printf(failures == 0 ? "ok\n" : "failed\n");
  return failures == 0 ? 0 : 1;
}
