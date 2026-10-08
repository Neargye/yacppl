// probe: H14a state_saver declares __cxa_get_globals() noexcept; conflicts with <cxxabi.h> if it declares it without noexcept
// where: macos-appleclang clang20-libcxx gcc14 gcc-m32
// std: c++11 c++14
// expect: build-fail "exception specification"
// meaning: build-fail confirms H14a for this ABI library; build-ok refutes it (libstdc++ declares it noexcept).
#include <cxxabi.h>

#include <state_saver.hpp>

int main() {
  int value = 0;
  {
    SAVER_EXIT(value);
    value = 1;
  }
  return value;
}
