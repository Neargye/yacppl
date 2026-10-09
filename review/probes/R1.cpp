// probe: R1 pre-C++17 uncaught_exceptions() via __cxa_get_globals matches the real count during unwinding (profile risk 1, control re-check of D1)
// where: macos-appleclang clang-libcxx gcc-latest gcc-m32
// std: c++11 c++14 c++17
// expect: run-ok
// meaning: run-fail at c++11/c++14 means the fixed offset reads the wrong field of __cxa_eh_globals for this ABI library (R1 materialised, D1 must be reopened); run-ok means the layout assumption holds for this job. c++17 uses std::uncaught_exceptions() and is the control: run-fail only at c++17 means the probe itself is wrong.
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
    check(1, "nested_thrower after inner catch");
  }
};

// A guard created inside a destructor that runs during unwinding must compare against the count at its construction.
struct fail_guard_in_unwinding {
  int* value;
  ~fail_guard_in_unwinding() {
    {
      SAVER_FAIL(*value);
      *value = 7;
    }
    if (*value != 7) {
      std::printf("saver_fail created during unwinding restored without a new exception\n");
      ++failures;
    }
    {
      SAVER_SUCCESS(*value);
      *value = 8;
    }
    if (*value != 7) {
      std::printf("saver_success created during unwinding did not restore on normal exit\n");
      ++failures;
    }
  }
};

} // namespace

int main() {
  {
    check_on_unwind outer = {0};
    static_cast<void>(outer);
  }
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
  check(0, "after all catches");

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
  try {
    SAVER_SUCCESS(value);
    value = 9;
    throw 1;
  } catch (int) {
  }
  if (value != 9) {
    std::printf("saver_success restored on exception\n");
    ++failures;
  }

  int unwinding_value = 0;
  try {
    fail_guard_in_unwinding guard = {&unwinding_value};
    static_cast<void>(guard);
    throw 1;
  } catch (int) {
  }

  std::printf("sizeof(void*)=%u, %s\n", static_cast<unsigned int>(sizeof(void*)), failures == 0 ? "ok" : "failed");
  return failures == 0 ? 0 : 1;
}
