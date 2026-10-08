# yacppl review report

| | |
|---|---|
| Revision reviewed | `c34df7597731193d163a86bf526f45ee1bafc94a` (master, "clean-up") |
| Review branch | `claude/sleepy-shannon-54vxw4` — review only, not for merging as is |
| Fix commits | `68b522b` fix attributes: annotate ATTR_FALLTHROUGH in C++98; `fcedabb` fix bytes: value-initialize in from_bytes<T> |
| Toolchains | g++ 13.3.0, clang++ 18.1.3 (libstdc++ 13), cmake 3.28.3, ninja 1.11.1 — Linux cloud container |
| Not available | MSVC, ClangCL, AppleClang, libc++/libc++abi, Clang sanitizer runtimes and libFuzzer, GCC ≥ 14, Clang ≥ 19, 32-bit multilib |
| Process | C++ review runbook: profile → analytical review (30 hypotheses) ∥ empirical checks (51 checks) → verdicts and fixes → acceptance → review CI (7 jobs, `2dd1925`) |
| Review CI | [run 37813288959](https://github.com/Neargye/yacppl/actions/runs/37813288959): macOS AppleClang, Clang 20 + libc++ + ASan/UBSan, GCC 14, GCC -m32, MSVC x64, MSVC x86, ClangCL — all green |

## Summary

- **Hypotheses:** 30 (bug 4, portability 12, compile-time 2, quality 12); 31 verdict rows (H14 split).
  After review CI: CONFIRMED 23 (H3 confirmed by CI; H14a moved to REFUTED), FORMAL 2, REFUTED 6
  (H7, H8, H14a, H14b, H15, plus H10 on the tested libraries), NOT CHECKED 0.
- **Empirical checks:** 51 — 42 passed, 3 failed (all in `attributes.hpp`), 3 informational, 3 impossible
  in this environment. Test runs: ctest 5 configurations × 24 = 120/120, manual matrix 36 configurations
  497/497, examples 78/78, randomized differential test 41.5 M iterations with 0 mismatches.
- **Fixed (2):** `ATTR_FALLTHROUGH` in C++98 (H4); `from_bytes<T>` SFINAE/instantiation mismatch (H20).
- **Owner questions (7):** `ATTR_LIKELY` type, `ATTR_ASSUME` on GCC, `ATTR_NODISCARD` before C++17 on GCC,
  `unforward` const, `CATCH_HANDLER` without SUPPRESS, C++11 examples, README notes.
- **No problems found** in `concepts`, `type_traits`, `state_saver` runtime behavior, `cmp_*`/`in_range`,
  `forward_like`, `move*`, `bit_cast`, `byte` operators — all match `std::*` on the tested inputs.
- **Main risk closed by review CI:** the pre-C++17 `uncaught_exceptions()` via `__cxa_get_globals` + fixed
  offset returns the correct count on libc++abi (macOS AppleClang, Clang 20 + libc++) and on 32-bit
  libstdc++ (probe R1, C++11/14, nested unwinding and `SAVER_FAIL`).
- **New confirmation from review CI:** `ATTR_ASSUME` stops compiling inside an expression in C++23 on
  GCC 14 and Clang 20 (H3) — strengthens Q2.

## Fixes

### F1. `ATTR_FALLTHROUGH` empty in C++98 (H4, empirical problem 3) — commit `68b522b`

- **Problem:** README declares `attributes` for C++98, but in C++98 the macro fell back to `/*fallthrough*/`,
  although GCC ≥ 7 and Clang ≥ 10 accept `__attribute__((__fallthrough__));` with `-pedantic-errors`.
- **Evidence:** `-std=c++98 -Wimplicit-fallthrough -pedantic-errors -Werror`: GCC "this statement may fall
  through", Clang "unannotated fall-through between switch labels"; control with `__has_attribute(__fallthrough__)`
  and `__attribute__((__fallthrough__));` compiles cleanly in c++98/c++03 on both.
- **Change:** `include/attributes.hpp` — helper `NEARGYE_ATTR_HAS_ATTRIBUTE` (same pattern as the existing
  `NEARGYE_ATTR_HAS_*` helpers, `#undef` at the end) and a branch
  `(__clang__ || __GNUC__) && NEARGYE_ATTR_HAS_ATTRIBUTE(__fallthrough__)` → `__attribute__((__fallthrough__));`.
  C++11+ and MSVC branches unchanged.
- **Test:** `test/attributes_cpp98_test.cpp` (exit-code test: doctest needs C++11) and target
  `yacppl-attributes-cpp98.t` (`CXX_STANDARD 98`, project options, `-Wimplicit-fallthrough` on GCC/Clang).
- **Old vs new:** old header — build error on GCC (12:7) and Clang (14:5); new — ctest passes, c++98/c++03 × GCC/Clang 4/4.
- **Residual:** GCC < 7, Clang < 10 and non-GNU compilers keep the no-op fallback; the new C++98 target on MSVC
  (which has no C++98 mode and uses its default standard) is verified only by CI.

### F2. `from_bytes<T>` accepted by SFINAE but fails to instantiate (H20) — commit `fcedabb`

- **Problem:** the constraint checks `is_default_constructible_v` (`T()`), the body used `T dst{}`; an aggregate
  with an explicit-default-constructor member passes detection and then hard-errors.
- **Evidence:** detector true on both compilers; the call fails — GCC "would use explicit constructor",
  Clang "chosen constructor is explicit in copy-initialization".
- **Change:** `include/byte.hpp:167` `T dst{};` → `T dst = T();` (guaranteed elision) + one-line comment;
  matches the existing `noexcept` specification.
- **Test:** `test/byte_test.cpp` — `explicit_default_member(_payload)`, two `static_assert`s, a `round_trip` check.
- **Old vs new:** `byte_test.cpp` does not compile with the old header (GCC/Clang, c++17/c++20); 9/9 test cases pass with the new one.

Old-code control (`ninja -k 0` on a copy with the old headers): exactly `attributes-cpp98`, `byte`, `byte-cpp20` fail
on GCC and Clang; everything else builds.

## Not fixed

| Item | Reason | Decision |
|---|---|---|
| H1 `ATTR_LIKELY`/`ATTR_UNLIKELY` have type `long` on GCC/Clang | changes the type of a public macro | D2, Q1 |
| H2/H3 `ATTR_ASSUME` is a no-op on GCC; expression vs statement differs between C++20 and C++23 (confirmed by review CI) | a full fix turns an expression into a statement; fixing only GCC 13 `-std=c++2b` is a half-measure | D3, D19, Q2 |
| Empirical problem 2 / H5 `ATTR_NODISCARD` on GCC C++11/14 | would reverse the owner's decision in 96cf183 | D4, Q3 |
| H6 `ATTR_NO_UNIQUE_ADDRESS` layout depends on `-std` | deliberate gating | D5 |
| H9 `NSTD_UNUSED` in a lambda capture | the macro intentionally does not evaluate its arguments | D7 |
| H18 `CATCH_HANDLER` without SUPPRESS silently ignored | `#error` would break configurations that build today | D13, Q5 |
| H19, H22 precondition violations in `byte.hpp` | undetectable at run time; README note suggested | D14 |
| H24 `invoke_each`/`apply_each` not usable in constant expressions in C++17 | `std::invoke` is constexpr only since C++20 | D15 |
| H25, H26 compile-time limits (Clang fold 256; `constexpr_for` N ≤ 447 GCC / 507 Clang) | no minimal reliable fix | D16 |
| H27 `unforward(const T&)` returns `const T` | intent unknown | D17, Q4 |
| H30 `cxx_std_11` examples built as C++17 | build-policy question | D18, Q6 |
| H17 dead sub-condition in a `static_assert` | no observable effect, no failing test possible | D12 |
| H11, H12, H16, H23, H28, H29 | design | D9 |
| H13, H21 | formal only | D10 |
| H7, H8, H10, H14a, H15 | refuted or not reproducible (review CI and container) | D6, D8, D11, D19 |

## Questions for the owner

- **Q1.** Should `ATTR_LIKELY`/`ATTR_UNLIKELY` yield `bool` everywhere? Ready patch:
  `static_cast<bool>(__builtin_expect(static_cast<bool>(x), 1))`.
- **Q2.** `ATTR_ASSUME` on GCC: (a) treat C++23 drafts as C++23 (`> 202002L`) — fixes only GCC 13 `-std=c++2b`;
  (b) GCC `__attribute__((__assume__(e)))` in all modes + "statement only" in README; (c) document the no-op.
  Review CI adds: the macro already changes from an expression to a statement in C++23 on GCC 14 and
  Clang 20 (`(ATTR_ASSUME(c), x)` fails to compile), so code valid in C++20 breaks in C++23 (H3).
  Whatever the choice, README should say "use as a statement only".
- **Q3.** `ATTR_NODISCARD` before C++17 on GCC: keep `__warn_unused_result__` (where `(void)` does not silence it
  and it warns on types), switch to `[[nodiscard]]`, or document the limitation?
- **Q4.** Is it intended that `unforward` keeps `const` on class prvalues (blocks moves: 2 copies, 0 moves)?
- **Q5.** Should `STATE_SAVER_CATCH_HANDLER` without `STATE_SAVER_SUPPRESS_THROW_RESTORE` be a compile error?
- **Q6.** Build examples with `CXX_STANDARD 11` so that C++11 branches are exercised in CI?
- **Q7.** Add README notes for H6, H9, H19/H22, H24, H25/H26?

## Verification after the fixes

| Configuration | Result |
|---|---|
| ctest: GCC Debug, Clang Debug, GCC ASan+UBSan Debug, GCC Release, Clang Release, GCC ASan+UBSan Release, Clang UBSan-trap | 7 × 25/25 |
| Each header standalone, minimum standard + c++2b (GCC) / c++2c (Clang), `-pedantic-errors -Werror` | 28/28 |
| Manual matrix: GCC c++11…23 Debug and `-O2 -DNDEBUG`, Clang c++11…2c Debug and `-O2 -DNDEBUG`, ASan, UBSan-trap, `-fno-rtti` | 33 configurations, 469/469 |
| `-fno-exceptions` probes | 120/120 |
| Examples | 50/50, identical output across builds |
| C++98 test by hand | 4/4 |
| Acceptance re-run on the branch HEAD (GCC Debug, Clang Debug, GCC ASan+UBSan) | 3 × 25/25 |

**CI:** no extension is required for the fixed issues — the new ctest target and `byte_test` run in all three
workflows. Optional: one Ubuntu GCC ASan+UBSan job; one GCC C++23 job if Q2 (a) or (b) is accepted.
Workflow files are unchanged.

## Review CI

Temporary workflow `.github/workflows/review.yml` and probes in `review/probes/` (commit `2dd1925`, review branch only).
Each probe states the hypothesis' prediction; `run.py` compares it with the actual outcome.
Run: https://github.com/Neargye/yacppl/actions/runs/37813288959 — 7/7 jobs green; project build + ctest green in every job.

| Job | Project build + ctest | Probes |
|---|---|---|
| macos-appleclang (libc++abi) | green | H14a build-ok (c++11/14); R1 ok (c++11/14) |
| clang20-libcxx (Clang 20, libc++ 20, ASan+UBSan) | green | H3: c++17 ok, c++23 build-fail; P1: c++17 `__builtin_assume(x)`, c++23 `[[assume(x)]]`; H10 c++2c build-ok; H14a build-ok; R1 ok |
| gcc14 | green | H3: c++17 ok, c++23 build-fail; P1: c++17 `static_cast<void>(0)` (no-op), c++23 `[[assume(x)]]`; H10 c++2c build-ok; H14a build-ok; R1 ok |
| gcc-m32 | green | H14a build-ok; R1 ok (32-bit `__cxa_eh_globals` offset correct) |
| msvc-x64 | green | H7: c++14 build-ok (no C4100), c++17 ok; H8: no LNK2005; P1: `__assume(x)` in c++17 and c++latest (`_MSVC_LANG` 202400) |
| msvc-x86 | green | H8: no LNK2005 |
| clangcl | green | H8: no LNK2005; P1: `__assume(x)` (c++latest reports `_MSVC_LANG` 202004) |

Conclusions:
- **R1 / profile risk 1 closed:** the pre-C++17 `uncaught_exceptions()` fallback is correct on libstdc++ (64- and 32-bit) and libc++abi (macOS, Linux).
- **H3 confirmed:** `ATTR_ASSUME` is an expression before C++23 and a statement in C++23 on GCC 14 and Clang 20 → Q2.
- **Empirical problem 1 bounded:** on GCC 14 `ATTR_ASSUME` works in C++23 (`[[assume]]`) but remains a no-op before C++23; GCC 13 is a no-op in every mode.
- **H7, H8, H14a refuted; H10** not reproducible with libstdc++ 14 and libc++ 20.
- **F1 on MSVC/ClangCL:** the new `yacppl-attributes-cpp98.t` target builds and passes on MSVC x64/x86 and ClangCL (default standard), and in the existing windows/macos/ubuntu workflows.

## Decisions

Decisions from this and earlier reviews. Later reviews do not re-propose them without a new argument.

| ID | Decision | Why | Reference |
|---|---|---|---|
| D1 | `__COUNTER__` inside other macros' arguments is not suppressed on Clang 22+ (`-Wc2y-extensions`) — known limitation, left as is | no reliable fix; suppressed by a flag in the project build | include/state_saver.hpp:307-312, test/CMakeLists.txt:39-41, example/CMakeLists.txt:13-15 (manual review) |
| D2 | `ATTR_LIKELY`/`ATTR_UNLIKELY` keep type `long` on GCC/Clang (`bool` on MSVC/fallback) | changing to `bool` alters `decltype`, overload resolution and deduction of a public macro — owner decision; ready fix: `static_cast<bool>(__builtin_expect(static_cast<bool>(x), 1))` | include/attributes.hpp:167,176; H1; Q1 |
| D3 | `ATTR_ASSUME` stays a no-op on GCC (all modes on GCC 13) | full fix (`__attribute__((__assume__(e)))` on GCC) turns an expression into a statement-only macro, breaking `(ATTR_ASSUME(c), x)`; fixing only GCC 13 `-std=c++2b` (`__cplusplus 202100L`) is a half-measure | include/attributes.hpp:118-128; empirical problem 1, H2/H3; Q2 |
| D4 | `ATTR_NODISCARD` on GCC C++11/14 stays `__warn_unused_result__` (`(void)` does not silence it, `-Wattributes` on types) | owner gated standard attributes by language version in 96cf183 ("fix attributes"); using `[[nodiscard]]` before C++17 reverses that decision | include/attributes.hpp:131-141; empirical problem 2, H5; Q3 |
| D5 | `ATTR_NO_UNIQUE_ADDRESS` stays C++20-only; layout differs between `-std` modes | explicit owner gating (MSVC ABI comment and links in the header); documentation note suggested only | include/attributes.hpp:193-205; H6, observation O2 |
| D6 | MSVC/ClangCL hypotheses H7 (`warning(suppress)` on multi-line functions) and H8 (`__forceinline` linkage) not checked | no MSVC toolchain on the review machine | H7/H8 |
| D7 | `NSTD_UNUSED` in a lambda capture still triggers Clang `-Wunused-lambda-capture` | the macro is documented as non-evaluating; making it odr-use the argument changes semantics; `nstd::unused` covers the case — documentation only | include/unused.hpp:46; README:43; H9 |
| D8 | `nstd::Trivial` uses `std::is_trivial` (deprecated in C++26) | not reproducible with libstdc++ 13; replacement changes semantics — deferred | include/concepts.hpp:121; H10 |
| D9 | H11 (category aliases strip references), H12 (`is_same_signedness` follows `is_signed`), H16 (pointers rejected by state_saver), H23 (`to_integer` default `unsigned char`), H28 (std-like names and ADL), H29 (`bit_cast` not constexpr) left as is | consistent design choices, partly locked by tests/README; H29 would be new functionality | concepts_test.cpp:174-183, type_traits_test.cpp:416, README:178 |
| D10 | H13 (ODR across `-std`/NDEBUG) and H21 (const member written by memcpy) left as is | formal only: `-flto -Wodr` silent, values identical, UBSan silent | H13/H21 |
| D11 | H14a (`__cxa_get_globals` declared `noexcept`) and H15 (`-fno-exceptions` ABI libs) left as is | conflict reproduced only on a mock; real libc++abi/libcxxrt not available; libstdc++ path verified | include/state_saver.hpp:65-70,104-107; H14/H15 |
| D12 | dead sub-condition in the `state_saver requires lvalue type` static_assert (H17) left as is | no observable effect, so no failing regression test is possible; cosmetic | include/state_saver.hpp:178; H17 |
| D13 | `STATE_SAVER_CATCH_HANDLER` without `STATE_SAVER_SUPPRESS_THROW_RESTORE` stays silently ignored | adding `#error` breaks currently compiling configurations — owner decision | include/state_saver.hpp:82-98; H18; Q5 |
| D14 | `from_bytes` into potentially-overlapping subobjects (H19) and negative counts with `sizeof(T)==1` (H22) left as is | precondition violations, undetectable at run time; documentation note suggested | include/byte.hpp:70,174-188; README:61; H19/H22 |
| D15 | `invoke_each`/`apply_each` are `constexpr` but usable in constant expressions only from C++20 (H24) | `std::invoke` is constexpr only since C++20; harmless for templates; documentation note suggested | include/utility.hpp:79,209,214; H24 |
| D16 | compile-time limits left as is: Clang fold limit 256 for `invoke_each`/`apply_each` (H25), `constexpr_for` max N=447 (GCC) / 507 (Clang) (H26) | no minimal reliable fix: every fold and the recursive trait would need rewriting; documentation note suggested | include/utility.hpp:47-80,136-154,276-285; H25/H26 |
| D17 | `unforward(const T&)` returns a `const T` prvalue for class types (H27) | intent unknown (the test locks only the scalar case) — owner question | include/utility.hpp:191-194; utility_test.cpp:389; Q4 |
| D18 | examples with `cxx_std_11` are built with `-std=c++17` by CMake (H30) | `target_compile_features` is a minimum by design; changing to `CXX_STANDARD 11` is a build-policy decision; C++11/14 example builds verified manually | example/CMakeLists.txt:22-39; empirical row 18, H30; Q6 |
| D19 | Review CI (run 37813288959) supersedes the "not checked" parts of D6, D8 and D11: H7, H8 and H14a are refuted, H10 does not reproduce with libstdc++ 14 / libc++ 20; H3 is confirmed and joins Q2 | checked on MSVC x64/x86, ClangCL, AppleClang/libc++abi, Clang 20 + libc++, GCC 14, GCC -m32 | review/probes/*, .github/workflows/review.yml (review branch only) |

## Hypothesis verdicts

Real project headers, GCC 13 and Clang 18. CONFIRMED — reproduced; REFUTED — does not reproduce;
FORMAL — formal violation without observable effect; NOT CHECKED — toolchain unavailable.

| H | Location / kind | Verdict | GCC 13 | Clang 18 |
|---|---|---|---|---|
| H1 `ATTR_LIKELY/UNLIKELY` have type `long` | attributes.hpp:158,167 / portability | CONFIRMED | `bool b{ATTR_LIKELY(c)}` → narrowing error from `long int` | same |
| H2 `ATTR_ASSUME` is a no-op on GCC | attributes.hpp:108-117 / portability | CONFIRMED | expands to `static_cast<void>(0)` in c++11/17/20/2b | `__builtin_assume` in every mode |
| H3 `ATTR_ASSUME` is a statement in C++23, an expression otherwise | attributes.hpp:110 / portability | CONFIRMED by review CI (GCC 14, Clang 20) | GCC 14 c++23: `attributes.hpp:121:32: error: expected identifier before '[' token`; c++17 ok | Clang 20 c++23: `error: expected variable name or 'this' in lambda capture list`; c++17 ok |
| H4 `ATTR_FALLTHROUGH` empty in C++98 | attributes.hpp:95-105 / portability | CONFIRMED → **fixed (F1)** | c++98 error, c++11 ok | c++98 error, c++11 ok |
| H5 `ATTR_NODISCARD` on types in C++11/14 | attributes.hpp:121-131 / portability | CONFIRMED for GCC; MSVC not checked | `-Werror=attributes` in c++11/14 | ok in every mode |
| H6 `ATTR_NO_UNIQUE_ADDRESS` only from C++20 | attributes.hpp:186-194 / portability | CONFIRMED (layout depends on `-std`) | sizeof 8/8/4 (c++11/17/20) | 8/8/4 |
| H7 MSVC `warning(suppress)` and multi-line functions | attributes.hpp:149 / portability | REFUTED by review CI | — | MSVC x64 `/W4 /WX` c++14 (fallback branch): builds, no C4100 |
| H8 `__forceinline` without `inline` | attributes.hpp:85-86 / portability | REFUTED by review CI | — | MSVC x64, MSVC x86, ClangCL, c++14/17: header definition in two TUs links, no LNK2005 |
| H9 `NSTD_UNUSED` on a lambda capture | unused.hpp:46 / quality | CONFIRMED on Clang | ok | `-Wunused-lambda-capture` |
| H10 `std::is_trivial` deprecated in C++26 | concepts.hpp:169 / portability | REFUTED on the tested libraries (review CI) | GCC 14 / libstdc++ 14, c++2c `-Werror`: builds | Clang 20 / libc++ 20, c++2c `-Werror`: builds |
| H11 category aliases strip references | concepts.hpp / quality | CONFIRMED (behavior), design | `Object<int&>` = `int&` | same |
| H12 `is_same_signedness` on bool/enum/char | type_traits.hpp:233 / quality | CONFIRMED (behavior), design | `<bool, unsigned>` = true; enums false | same |
| H13 ODR across TUs with different `-std` | type_traits.hpp / quality | FORMAL | `-flto -Wodr` silent, correct result | — |
| H14a `__cxa_get_globals() noexcept` vs `<cxxabi.h>` | state_saver.hpp:65-69 / portability | REFUTED by review CI (conflict existed only on a mock) | GCC 14, GCC -m32: `<cxxabi.h>` + state_saver builds in c++11/14 | macOS AppleClang (libc++abi), Clang 20 + libc++abi 20: builds in c++11/14 |
| H14b `__cxa_get_globals` path in gnu++11/14 | state_saver.hpp:104-107 / portability | REFUTED (container) + review CI probe R1 | equals `std::uncaught_exceptions()`; R1 ok on GCC 14 and GCC -m32 | R1 ok on macOS AppleClang (libc++abi) and Clang 20 + libc++ (ASan/UBSan) |
| H15 `-fno-exceptions` and `__cxa_get_globals` | state_saver.hpp:104-107 / portability | REFUTED on libstdc++ | builds and runs | same |
| H16 pointers rejected, member pointers accepted | state_saver.hpp:182-183 / quality | CONFIRMED (behavior), design | as described | same |
| H17 dead part of a static_assert condition | state_saver.hpp:178 / quality | CONFIRMED (logically) | equivalent on 14 types | — |
| H18 `CATCH_HANDLER` without SUPPRESS ignored | state_saver.hpp:82-98 / quality | CONFIRMED | builds silently | same |
| H19 `from_bytes(T&)` into a base subobject overwrites a derived member placed in its tail padding | byte.hpp:173-188 / bug | CONFIRMED (precondition) | derived member 42 → 0 | same |
| H20 `from_bytes<T>` SFINAE yes, instantiation error | byte.hpp:166 / bug | CONFIRMED → **fixed (F2)** | "would use explicit constructor" | "chosen constructor is explicit" |
| H21 memcpy into a type with a const member | byte.hpp, utility.hpp:203 / bug | FORMAL | UBSan silent, values correct | same |
| H22 negative count with `sizeof(T)==1` | byte.hpp:70 / quality | CONFIRMED (precondition) | `-DNDEBUG`: SIGSEGV for `unsigned char` | — |
| H23 `to_integer` defaults to `unsigned char` | byte.hpp:91-94 / quality | CONFIRMED (behavior), design | prints `A` | — |
| H24 `invoke_each`/`apply_each` not constexpr in C++17 | utility.hpp:208-216 / portability | CONFIRMED | c++17 error, c++20 ok | same |
| H25 fold-expression limit | utility.hpp:47-80,210 / compile-time | CONFIRMED on Clang | N=1000 ok | N=257: nesting limit 256 exceeded |
| H26 `constexpr_for` recursion limit | utility.hpp:136-148,276-285 / compile-time | CONFIRMED (not in README) | max N=447 (depth 900) | max N=507 (depth 1024) |
| H27 `unforward(const&)` returns a const prvalue | utility.hpp:191-194 / bug | CONFIRMED (behavior), owner question | `const std::string` | same |
| H28 ADL ambiguity of `move` | utility.hpp / quality | CONFIRMED (behavior), design | ambiguous call | same |
| H29 `bit_cast` not constexpr | utility.hpp:201-206 / quality | CONFIRMED (behavior), design | non-constexpr call | same |
| H30 `cxx_std_11` examples built with `-std=c++17` | example/CMakeLists.txt / quality | CONFIRMED | `-std=c++17` in compile_commands.json | — |

## Empirical checks (before the fixes)

| # | Check | Configuration | Result |
|---|---|---|---|
| 1–2 | T1 baseline build + ctest | GCC 13 / Clang 18, Debug | PASS 24/24, no warnings |
| 3–4 | T1 standalone headers | minimum standard (98/11/17) + c++2b (GCC) / c++2c (Clang), `-pedantic-errors -Werror` | PASS 14/14 each |
| 5 | T1 sanitizers | GCC, ASan+UBSan, `-fno-sanitize-recover=all` | PASS 24/24 (runtime presence confirmed with `nm`) |
| 6–7 | T2 Release | GCC / Clang | PASS 24/24 |
| 8–11 | All tests on every standard (including concepts/unused/type_traits/7 state_saver modes that CMake runs only on C++11) | GCC c++11…23, Clang c++11…2c; Debug and `-O2 -DNDEBUG` | PASS 71/71, 86/86, 71/71, 86/86 |
| 12 | UBSan without runtime | Clang `-fsanitize=undefined -fsanitize-trap=undefined`, c++11/17/20/2c | PASS 58/58 |
| 13–14 | ASan+UBSan on other standards, Release | GCC c++11/14/23 Debug; c++11/17/20 `-O2 -DNDEBUG` | PASS 41/41, 43/43 |
| 15 | P6 `-fno-rtti` | GCC/Clang, c++11/17 | PASS 56/56 |
| 16 | P6 `-fno-exceptions` (all headers, SAVER_*, WITH_SAVER, byte, constexpr_for) | GCC/Clang × c++11…20 × 6 state_saver modes × 3 flag sets | PASS 120/120; 24 builds with `NO_THROW_RESTORE` rejected by the expected static_assert |
| 17 | Examples run | 5 builds | PASS 50/50, byte-identical output |
| 18 | Examples on C++11/14 (CMake builds them as C++17) | GCC/Clang × c++11/14 × 7 examples | PASS 28/28, same output as C++17 |
| 19 | Strict warnings (172 TUs) | GCC `-Wconversion -Wsign-conversion -Wshadow -Wold-style-cast -Wuseless-cast …`; Clang `-Weverything` | PASS; in `include/` only the intentional `-Wreserved-identifier` (`__cxa_get_globals`) and `-Wuseless-cast` from `ATTR_LIKELY` on a bool argument in user code |
| 20 | Strict warnings on all integer pairs for `cmp_*`/`in_range` | GCC c++23, Clang c++2c | PASS, 0 warnings |
| 21 | Expansion of every `ATTR_*` | GCC 98…23, Clang 98…26 | INFO (table in O7) |
| 22 | `ATTR_ASSUME` takes effect | GCC c++11/17/20/23 | **FAIL** — problem 1 |
| 23 | `ATTR_ASSUME` takes effect | Clang c++11…23 | PASS (`movl $5, %eax`) |
| 24 | `ATTR_NODISCARD` | GCC c++98/11/14 | **FAIL** — problem 2 |
| 25 | `ATTR_NODISCARD` | GCC c++17+, Clang all | PASS |
| 26 | `ATTR_FALLTHROUGH` | GCC/Clang c++98 | **FAIL** — problem 3 (fixed in F1) |
| 27 | `ATTR_FALLTHROUGH` | c++11…23 | PASS |
| 28 | `ATTR_ALWAYS_INLINE`, `NORETURN`, `DEPRECATED`, `MAYBE_UNUSED` | GCC/Clang 98…23 | PASS (checked in assembly and diagnostics) |
| 29 | `ATTR_NODISCARD_MSG` | c++20/23 | PASS (message shown; none before C++20, as documented) |
| 30 | `ATTR_TRIVIAL_ABI` | Clang / GCC | PASS (register passing on Clang; empty on GCC, as documented) |
| 31 | `ATTR_NO_UNIQUE_ADDRESS` | both | PASS in c++20/23 (`sizeof` 4); 8 in c++11/17 (O2) |
| 32 | `ATTR_LIKELY/UNLIKELY` | both | PASS (single evaluation); result type `long` (O1) |
| 33 | `cmp_*`/`in_range` vs `std::` | 10×10 integer types × 7 edge values, static_assert | PASS; SFINAE rejection of bool/char types/float/pointer/byte matches [utility.intcmp] |
| 34 | `forward_like` (vs P2445), `move`, `move_if_noexcept`, `forward`, `to_underlying`, `decay_copy` | GCC c++23, Clang c++2c | PASS |
| 35 | type_traits and concepts vs std on cv/references/arrays/functions/abominable functions/member pointers | same | PASS |
| 36 | `is_nothrow_convertible` std branch and backport vs std | ≈ 230 From/To pairs | PASS |
| 37 | `nstd::byte` vs `std::byte`: bitwise ops 256×256, shifts 256×32, `to_integer` | same | PASS |
| 38 | `bit_cast` vs `std::bit_cast` (±0, inf, NaN, denormals, max) | same | PASS, 0 mismatches |
| 39 | `constexpr_for<0,N,1>` limits | GCC / Clang, c++17 | INFO: max N=446 / 507; N=400 compiles in 1.25 s / 187 MB (GCC), 1.04 s / 137 MB (Clang) |
| 40 | `constexpr_for` extreme values | both | PASS (INT_MAX/INT_MIN/LLONG_MAX ranges, unsigned char; `Inc` ≤ 0, bool, mixed types rejected by SFINAE) |
| 41 | P2 try_compile "throwing destructor" by diagnostic text | GCC/Clang × c++11…23 | PASS (positive control clean; negative — exactly the stated static_assert) |
| 42 | P2 `#error` on conflicting macros + other state_saver static_asserts | both | PASS 5/5 and 18/18, first error is the stated one |
| 43 | P3 `__cxa_get_globals` + offset vs `std::uncaught_exceptions` | GCC/Clang × gnu++11/14 × -O0/-O2, nesting up to 5, rethrow | PASS 8/8, 0 mismatches |
| 44 | P3 `__COUNTER__`/WITH_SAVER nesting, break/continue | both × c++11/17/2b, `-Wshadow -Werror` | PASS; `break` inside WITH_SAVER leaves the macro's loop — documented (O5) |
| 45 | P5 edge inputs for `to_bytes`/`from_bytes`/shifts | GCC ASan+UBSan, Debug and `-O2 -DNDEBUG` | PASS — behavior matches README (zero count, overflow rejection, null pointers, overlap, shift preconditions) |
| 46 | P5 randomized differential test (overlapping copies vs `memmove`, `cmp_*`/`in_range` vs std) | GCC c++20 ASan+UBSan, 120 s | PASS: 41,472,000 iterations, 0 mismatches |
| 47 | P7 hostile `windows.h` macros (`min`, `max`, `small`, `interface`, `byte` typedef, …) and 24 POSIX headers before the library | GCC/Clang × c++11…23 | PASS |
| — | P6 ODR with different config macros per TU | GCC c++17, -O0/-O2, both link orders, `-flto -Wodr` | INFO: behavior depends on link order and optimization, no diagnostic (O4); the requirement is documented |
| — | Unmasking suppressions | — | NOT POSSIBLE: all suppressions are MSVC-only or Clang ≥ 22 |
| — | 32-bit `__cxa_eh_globals` offset | — | NOT POSSIBLE: no multilib |
| — | libFuzzer | — | NOT POSSIBLE: no `libclang_rt.fuzzer` (replaced by row 46) |

### Confirmed problems

1. **`ATTR_ASSUME` is a no-op on GCC 13 in every mode, including C++23.** The `[[assume]]` branch needs
   `__cplusplus >= 202302L`, but GCC 13 reports `202100L` for `-std=c++23` while supporting `[[assume]]`;
   GCC has no `__builtin_assume`, so the fallback `static_cast<void>(0)` is used, and the available
   `__attribute__((assume(e)))` is not used before C++23. Assembly: with the macro GCC keeps `movl %edi, %eax`;
   the `[[assume]]` control gives `movl $5, %eax`. → D3, Q2.
2. **`ATTR_NODISCARD` on GCC in C++98/11/14 is not `[[nodiscard]]`:** `(void)f()` does not silence
   `__warn_unused_result__`, so code clean on C++17 and Clang fails with `-Werror` on GCC C++11/14; on a type
   GCC emits `-Wattributes` and the attribute has no effect. → D4, Q3.
3. **`ATTR_FALLTHROUGH` is empty in C++98 on GCC and Clang.** → fixed in F1.

### Observations

- **O1** `ATTR_LIKELY/UNLIKELY` have type `long` on GCC/Clang, `bool` in the fallback.
- **O2** `ATTR_NO_UNIQUE_ADDRESS`: `sizeof` 8 in c++11/17 and 4 in c++20/23 — layout depends on `-std`.
- **O3** `constexpr_for` limit N=446 (GCC 13) / 507 (Clang 18) at default template depth; not in README.
- **O4** Different `STATE_SAVER_*` macros in different TUs silently change behavior depending on link order and
  optimization; `-flto -Wodr` is silent. README requires identical macros.
- **O5** `break` inside `WITH_SAVER_*` leaves the macro's internal loop (documented in README).
- **O6** With the `__LINE__` fallback for `NEARGYE_STATE_SAVER_COUNTER`, two `SAVER_EXIT` on one line collide;
  relevant only for compilers without `__COUNTER__`.
- **O7** `ATTR_*` expansions: GCC C++98 uses `__attribute__` everywhere (FALLTHROUGH empty before F1, ASSUME a
  no-op); GCC C++11/14 — `[[noreturn]]`, NODISCARD `__warn_unused_result__`; GCC C++17+ — standard attributes,
  NO_UNIQUE_ADDRESS from C++20, ASSUME always a no-op; Clang — TRIVIAL_ABI `[[clang::trivial_abi]]`, ASSUME
  `__builtin_assume` in every mode.

### Not verified in the container (see Review CI for what CI closed)

- Closed by review CI: MSVC x64/x86, ClangCL, AppleClang/libc++abi, Clang 20 + libc++ with ASan/UBSan,
  GCC 14, 32-bit; `__forceinline`, `warning(suppress)`, `__assume`, the `__cxa_get_globals` path and its
  declaration; the positive `[[assume]]` branch (GCC 14 and Clang 20 in C++23).
- Still not verified: libFuzzer; GCC ≥ 15 / libstdc++ ≥ 15 (H10); `msvc::no_unique_address` layout;
  compiler-version boundaries below the CI versions.
