// probe: R1 state_saver redeclares __cxxabiv1::__cxa_get_globals() noexcept; the ABI library's <cxxabi.h> included first must not conflict (profile risk 1, second part)
// where: macos-appleclang clang-libcxx gcc-latest gcc-m32
// std: c++11 c++14
// expect: run-ok
// meaning: build-fail mentioning __cxa_get_globals (exception specification / conflicting declaration) means users who include <cxxabi.h> before <state_saver.hpp> cannot build pre-C++17 with this ABI library; run-fail means the count is wrong after the redeclaration; run-ok means no conflict for this job.
#include <cxxabi.h>

#include <state_saver.hpp>

#include <cstdio>

namespace {

struct check_on_unwind {
  int expected;
  int* failures;
  ~check_on_unwind() {
    if (nstd::detail::uncaught_exceptions() != expected) {
      ++*failures;
    }
  }
};

} // namespace

int main() {
  int failures = 0;
  try {
    check_on_unwind check = {1, &failures};
    static_cast<void>(check);
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
    ++failures;
  }
  std::printf(failures == 0 ? "ok\n" : "failed\n");
  return failures == 0 ? 0 : 1;
}
