// probe: R4 every attribute macro in its documented role builds warning-free and behaves on MSVC x86 and ClangCL (ClangCL defines both __clang__ and _MSC_VER, so branches mix) (profile risk 4)
// where: msvc-x64 msvc-x86 clangcl
// std: c++14 c++17 c++20
// expect: run-ok
// meaning: build-fail (C4xxx under /W4 /WX, or a clang-cl -W diagnostic) means one attribute macro branch is not usable on this compiler and standard; run-fail means a macro changed behaviour (fallthrough, likely/unlikely, no_unique_address size from C++20); a C4100/C4189 failure only at c++14 on msvc duplicates H11 (ATTR_MAYBE_UNUSED pre-C++17 fallback); run-ok means R4 does not materialise for this job.
#include <attributes.hpp>

#include <cstdio>

// External linkage, never called: calling it must warn, defining it must not.
ATTR_DEPRECATED("use twice") int old_twice(int x) {
  return 2 * x;
}

namespace {

struct empty_tag {};

struct holder {
  ATTR_NO_UNIQUE_ADDRESS empty_tag tag;
  int value;
};

struct ATTR_TRIVIAL_ABI handle {
  int value;
  explicit handle(int v) : value(v) {}
  handle(const handle& other) : value(other.value) {}
  ~handle() {}
};

int read_handle(handle h) {
  return h.value;
}

ATTR_NORETURN void stop() {
  throw 0;
}

ATTR_ALWAYS_INLINE int twice(int x) {
  return 2 * x;
}

ATTR_NODISCARD int plus_one(int x) {
  return x + 1;
}

ATTR_NODISCARD_MSG("use the sum") int plus_two(int x) {
  return x + 2;
}

int unused_parameter(int used, ATTR_MAYBE_UNUSED int unused) {
  return used;
}

int fallthrough(int x) {
  int r = 0;
  switch (x) {
    case 1:
      r += 1;
      ATTR_FALLTHROUGH
    case 2:
      r += 2;
      break;
    default:
      break;
  }
  return r;
}

int halve_positive(int x) {
  ATTR_ASSUME(x >= 0);
  return x / 2;
}

} // namespace

// A call keeps MSVC C4127 (conditional expression is constant) out of the size check.
unsigned int holder_size() {
  return static_cast<unsigned int>(sizeof(holder));
}

int main(int argc, char**) {
  int failures = 0;
  ATTR_MAYBE_UNUSED int unused_local = 1;
  if (fallthrough(1) != 3 || fallthrough(2) != 2) {
    std::printf("fallthrough changed control flow\n");
    ++failures;
  }
  if (ATTR_UNLIKELY(argc > 100)) {
    stop();
  }
  if (!ATTR_LIKELY(argc > 0)) {
    std::printf("ATTR_LIKELY changed the condition\n");
    ++failures;
  }
  if (twice(2) != 4 || plus_one(1) != 2 || plus_two(1) != 3 || unused_parameter(5, 0) != 5 || halve_positive(8) != 4) {
    std::printf("function attribute changed a result\n");
    ++failures;
  }
  if (read_handle(handle(42)) != 42) {
    std::printf("trivial_abi handle passed incorrectly\n");
    ++failures;
  }
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L
  if (holder_size() != sizeof(int)) {
    std::printf("no_unique_address has no effect in C++20: sizeof(holder)=%u\n", static_cast<unsigned int>(sizeof(holder)));
    ++failures;
  }
#endif
  std::printf("sizeof(holder)=%u, %s\n", static_cast<unsigned int>(sizeof(holder)), failures == 0 ? "ok" : "failed");
  return failures == 0 ? 0 : 1;
}
