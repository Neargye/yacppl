// probe: R5 byte/utility after the real <windows.h> (byte typedef, min/max macros) and with a 32-bit std::size_t: count overflow checks, shifts, bit_cast, integer comparisons (profile risk 5)
// where: msvc-x64 msvc-x86 clangcl gcc-m32
// std: c++17 c++20
// expect: run-ok
// meaning: build-fail after <windows.h> means a name or macro conflict (byte, min, max); run-fail means a 32-bit or Windows specific result is wrong (an overflowing count copied memory, a size_t comparison or a bit_cast differs); run-ok means R5 does not materialise for this job.
// NDEBUG: overflowing counts are documented no-ops in release builds; with asserts on they abort by design.
#define NDEBUG

#if defined(_WIN32)
#  include <windows.h>
#endif

#include <byte.hpp>
#include <utility.hpp>

#include <cstdint>
#include <cstdio>
#include <limits>

namespace {

int failures = 0;

void expect(bool ok, const char* what) {
  if (!ok) {
    std::printf("failed: %s\n", what);
    ++failures;
  }
}

} // namespace

int main() {
#if defined(_WIN32)
  // The global byte typedef of <rpcndr.h> must keep working next to nstd::byte.
  const byte legacy = 0x5A;
  expect(nstd::to_integer<int>(nstd::to_byte(legacy)) == 0x5A, "windows byte typedef next to nstd::byte");
#endif

  const std::size_t max_size = (std::numeric_limits<std::size_t>::max)();
  const std::uint32_t source[2] = {0x11223344u, 0x55667788u};
  nstd::byte buffer[sizeof(source)] = {};

  // count * sizeof(T) wraps around in size_t: must be rejected, buffer untouched.
  nstd::to_bytes(buffer, source, max_size / sizeof(std::uint32_t) + 1);
  expect(nstd::to_integer<int>(buffer[0]) == 0 && nstd::to_integer<int>(buffer[7]) == 0, "to_bytes with an overflowing count copied");

  std::uint32_t target[2] = {0, 0};
  nstd::to_bytes(buffer, source);
  nstd::from_bytes(target, buffer, max_size / sizeof(std::uint32_t) + 1);
  expect(target[0] == 0 && target[1] == 0, "from_bytes with an overflowing count copied");
  nstd::from_bytes(target, buffer);
  expect(target[0] == source[0] && target[1] == source[1], "from_bytes round trip");
  expect(nstd::from_bytes<std::uint32_t>(buffer + 4) == source[1], "from_bytes<T> round trip");

  // Shift counts of 64-bit types are validated before the cast to unsigned int.
  expect(nstd::detail::is_valid_byte_shift(std::size_t{7}), "size_t shift 7 is valid");
  expect(!nstd::detail::is_valid_byte_shift(std::size_t{32}), "size_t shift 32 is invalid");
  expect(!nstd::detail::is_valid_byte_shift(static_cast<std::uint64_t>(1) << 32), "uint64 shift 2^32 is invalid");
  expect(nstd::to_integer<int>(nstd::byte{1} << std::uint64_t{7}) == 0x80, "byte shift by uint64 7");

  expect(nstd::bit_cast<std::uint32_t>(1.0f) == 0x3F800000u, "bit_cast float");
  expect(nstd::bit_cast<std::uint64_t>(1.0) == 0x3FF0000000000000ull, "bit_cast double");

  // Mixed-signedness comparisons with a 32-bit size_t and with long of different widths.
  expect(nstd::cmp_less(-1, std::size_t{0}), "cmp_less(-1, size_t 0)");
  expect(!nstd::in_range<std::size_t>(-1), "in_range<size_t>(-1)");
  expect(nstd::in_range<std::size_t>(std::uint64_t{0xFFFFFFFFull}), "in_range<size_t>(2^32 - 1)");
  expect(nstd::in_range<std::size_t>(std::uint64_t{0x100000000ull}) == (sizeof(std::size_t) > 4), "in_range<size_t>(2^32)");
  expect(nstd::in_range<long>(std::uint32_t{0xFFFFFFFFu}) == (sizeof(long) > 4), "in_range<long>(2^32 - 1)");
  expect(nstd::cmp_greater(std::uint32_t{0x80000000u}, (std::numeric_limits<int>::max)()), "cmp_greater(2^31, INT_MAX)");

  std::printf("sizeof(size_t)=%u sizeof(long)=%u, %s\n", static_cast<unsigned int>(sizeof(std::size_t)), static_cast<unsigned int>(sizeof(long)),
              failures == 0 ? "ok" : "failed");
  return failures == 0 ? 0 : 1;
}
