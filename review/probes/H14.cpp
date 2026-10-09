// probe: H14 SAVER_EXIT and WITH_SAVER_EXIT expand __COUNTER__ in user code, which Clang >= 22 diagnoses as a C2y extension under -pedantic-errors
// where: clang-libcxx
// std: c++11 c++17
// expect: build-fail "c2y-extensions"
// meaning: build-fail mentioning c2y-extensions = user code built with -pedantic-errors cannot use the SAVER_* macros on this Clang (H14 confirmed); build-ok = this Clang does not diagnose __COUNTER__ (expected for Clang < 22: check the clang version in the job log before calling H14 refuted).
#include <state_saver.hpp>

int main() {
  int value = 1;
  {
    SAVER_EXIT(value);
    value = 2;
  }
  WITH_SAVER_EXIT(value) {
    value = 3;
  }
  return value - 1;
}
