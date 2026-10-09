# yacppl review report (2026-10-09)

Base revision `c34df7597731193d163a86bf526f45ee1bafc94a` (`master`). Review branch `claude/cool-volta-r8jtzw`.
Fix commits were made by step 3 in an isolated worktree and moved to the review branch by cherry-pick (hashes below are
the review-branch hashes). Toolchains: g++ 13.3.0, clang++ 18.1.3, libstdc++ 13, cmake 3.28.3, ninja 1.11.1. Nothing
was installed. Control run: decisions of previous reviews were not imported except D1.

"Fails on old / passes on new" means: the regression test was compiled and run against the base headers
(`git archive c34df75 include`) and against the patched headers, both with the project warning set
`-Wall -Wextra -pedantic-errors -Werror`.

```
$ git log --oneline c34df75..HEAD   (fix and proposal commits)
9fc253b [proposal] attributes: make ATTR_LIKELY and ATTR_UNLIKELY yield bool
98c9497 [proposal] utility: unforward returns a cv-unqualified value
1c6c6b6 [proposal] attributes: use [[nodiscard]] on gcc from c++11
1190716 byte: validate shift counts in their own type
9c794e8 state_saver: accept rvalue reference variables in guard macros
6c6a1e2 attributes: annotate fallthrough in c++98
```

The three fix commits come first. Each [proposal] commit applies on top of the fixes alone and builds and passes on its
own (fixes-only tree and fixes + each single proposal, g++ and clang++ Debug, ctest 26/26 each).

## Fixes

### 6c6a1e2 attributes: annotate fallthrough in c++98 (H6, evidence C1)

- Problem: in C++98/03 `ATTR_FALLTHROUGH` expanded to nothing (the `/*fallthrough*/` comment is removed before expansion),
  so a marked fallthrough was still diagnosed. g++ `-Wextra` enables `-Wimplicit-fallthrough`. The README promises C++98 support.
- Evidence: g++ 13 `-std=c++98|c++03 -Wall -Wextra -Werror`: `error: this statement may fall through
  [-Werror=implicit-fallthrough=]`. clang++ 18 with `-Wimplicit-fallthrough`: `unannotated fall-through between switch labels`.
- Fix: `#elif (defined(__clang__) || defined(__GNUC__)) && NEARGYE_ATTR_HAS_ATTRIBUTE(fallthrough)` ->
  `__attribute__((__fallthrough__));`. New internal helper `NEARGYE_ATTR_HAS_ATTRIBUTE`, defined and undefined exactly like
  `NEARGYE_ATTR_HAS_BUILTIN`.
- Test: new `test/attributes_cpp98_test.cpp` (plain `main`, because doctest needs C++11), target `yacppl-attributes-cpp98.t`
  (`-std=c++98 -Wimplicit-fallthrough`, GCC/Clang only). Old: BUILD-FAIL on g++ and clang++ at c++98 and c++03 with the
  diagnostics above. New: build + run rc=0. Step 2's all-macros probe (which failed at g++ c++98/03) now builds on
  g++ c++98..c++23 and clang++ c++98..c++2c.
- Residual: compilers without `__has_attribute(fallthrough)` (GCC < 7, Clang < 10, others) keep the empty fallback.
  AppleClang builds the new target only in the existing macOS CI.

### 9c794e8 state_saver: accept rvalue reference variables in guard macros (H12)

- Problem: `SAVER_*`, `MAKE_SAVER_*` and `WITH_SAVER_*` use `decltype(x)`. For a variable of rvalue reference type
  (a `std::string&& s` parameter, a forwarding `T&&` bound to an rvalue, `auto&&`) that type is `T&&`, and the class rejects
  it with "state_saver requires lvalue type.", although the variable is an lvalue and `nstd::saver_exit<T&>{x}` works.
- Evidence: g++ 13 / clang++ 18 c++11..c++2c: `state_saver.hpp:178: error: static assertion failed: state_saver requires
  lvalue type.`.
- Fix: the macros wrap the type in `nstd::detail::state_saver_lvalue_t<decltype(x)>`, which maps `U&&` to `U&` and passes
  every other type through unchanged, so existing guards keep exactly their types. The class is unchanged.
- Test: `state_saver_test.cpp` "state_saver macros accept variables of rvalue reference type" (`SAVER_EXIT` on an `int&&`
  parameter, `MAKE_SAVER_FAIL` with an exception, `WITH_SAVER_SUCCESS` on a forwarding reference bound to an rvalue and to
  an lvalue, `SAVER_EXIT` on a local `int&&`). Old: BUILD-FAIL with the static assertion on g++ c++11..c++23 and clang++
  c++11..c++2c. New: doctest SUCCESS everywhere. Runs in the c++11/14/17/20 targets and in cxxabi-first.
- Still rejected (checked on g++/clang++ c++11): `SAVER_EXIT(std::move(v))`, `WITH_SAVER_FAIL(std::move(v))` and
  `SAVER_EXIT(f())` (deleted `T&&` constructor); `SAVER_EXIT(const_var)` and `SAVER_EXIT(const_rref_var)` ("requires not
  const type").
- Residual: for `SAVER_EXIT(std::move(v))` the first diagnostic is now "use of deleted function ... saver_exit(T&&)"
  instead of "state_saver requires lvalue type.". The code is still rejected, and no repository test pins that text.

### 1190716 byte: validate shift counts in their own type (H20)

- Problem: `detail::is_valid_byte_shift` converted the count to `unsigned long long` before validation. For `__int128`
  counts (integral in GNU dialects, and CMake's default is `gnu++17`), `2^64 + 1` was truncated to 1, passed the assert,
  and the shift used count 1.
- Evidence: g++/clang++ `-std=gnu++17|gnu++20`: `static assertion failed: wide extended-integer shift counts must not be
  truncated before validation`. At run time `byte{1} << (2^64 + 1)` returned `2` with rc=0 and a silent assert (step 2b).
- Fix: `return shift < static_cast<I>(std::numeric_limits<unsigned int>::digits);`. Negative signed counts are rejected
  before this line, `bool` is excluded by the operator constraint, and the width fits every other integral type.
- Test: `byte_test.cpp` gets static checks for `__int128` / `unsigned __int128` counts (only where `__SIZEOF_INT128__` is
  defined and the type is integral), and a new target `yacppl-byte-gnu.t` builds `byte_test.cpp` at `-std=gnu++17` on
  GCC/Clang. Old: BUILD-FAIL at gnu++17/gnu++20 on both compilers. New: SUCCESS. Strict c++17/20/23 are unchanged (the
  check is skipped there). At run time the `2^64 + 1` shift now aborts on the assert (rc=134) on both compilers. Results
  and warnings are unchanged (also with `-Wconversion -Wsign-conversion`) for `char`, `signed/unsigned char`, `short`,
  `int`, `long`, `long long`, `wchar_t`, `char16_t`, `char32_t` and `int8_t` counts.
- Residual: under `NDEBUG` an out-of-range count is still not checked (documented precondition, D17).

## Proposals

### 1c6c6b6 [proposal] attributes: use [[nodiscard]] on gcc from c++11 (H1 med, H2 low; evidence C3)

- What changes: on GCC (not Clang) in C++11/14, `ATTR_NODISCARD` becomes `[[nodiscard]]` (when
  `__has_cpp_attribute(nodiscard)` is set) instead of `__attribute__((__warn_unused_result__))`.
- Why: GCC cannot silence `warn_unused_result` with a cast to void. So `static_cast<void>(f())` broke `-Werror` builds in
  C++11/14 (`ignoring return value ... declared with attribute 'warn_unused_result' [-Werror=unused-result]`), and
  `struct ATTR_NODISCARD S` gave `-Wattributes` (an error under `-Werror`). g++ 13 accepts `[[nodiscard]]` silently in
  C++11 with `-pedantic-errors`. Discarded calls are still diagnosed.
- Test: `attributes_test.cpp` discards nodiscard results with `static_cast<void>` and marks a class with `ATTR_NODISCARD`.
  Old: g++ c++11/c++14 BUILD-FAIL (one `-Wattributes` and two `-Wunused-result` errors). New: SUCCESS on g++/clang++
  c++11..c++23.
- Why a proposal and not a fix: it changes which code compiles. A trailing placement `int f() ATTR_NODISCARD;` compiled
  on GCC C++11/14 before and is now rejected, exactly as on GCC C++17 with the base header (H9).
- Risks: the placement change above. A GCC version that pedwarns on C++17 attributes in C++11 mode would break
  `-pedantic-errors` builds (GCC 13 does not; GCC 12/14 build this test in the existing ubuntu CI). GCC in C++98 keeps the
  GNU spelling, so `struct ATTR_NODISCARD S` still warns there.

### 98c9497 [proposal] utility: unforward returns a cv-unqualified value (H16 med, H17 unforward part)

- What changes: `nstd::unforward` returns `std::remove_cv_t<std::remove_reference_t<T>>`, with the constructibility and
  noexcept checks changed to match. Only class types are affected (scalar prvalues already dropped cv).
- Why: for a const class lvalue it returned a `const T` prvalue that cannot be moved from (copy assignment instead of move,
  and move-only-assignable types did not compile). For a volatile lvalue the volatile return type is deprecated in C++20
  (clang `-Wdeprecated-volatile` error at `utility.hpp:192`). `decay_copy` already returns a non-const value.
- Test: `utility_test.cpp` static_asserts (`const std::string&` -> `std::string`, `volatile int&` -> `int`) and runtime
  checks. Old: BUILD-FAIL on g++ c++17/20/23 and clang++ c++17/20/23/2c. New: SUCCESS. Step 2b probes on the patched
  header: `copy_assigns=0 move_assigns=2` (was 1/1).
- Risks: code relying on the const return type (overloads on `const T&&`, `decltype` checks) now sees `T`.

### 9fc253b [proposal] attributes: make ATTR_LIKELY and ATTR_UNLIKELY yield bool (H3 low)

- What changes: on GCC/Clang the macros become `(__builtin_expect(static_cast<bool>(x), 1|0) != 0)`, of type `bool`
  (was `long`). This matches the fallback `(static_cast<bool>(x))` used by every other compiler.
- Why: `auto`, `decltype`, overloads, deduction and `std::boolalpha` output differed by compiler (`1` vs `true`).
- Hint preserved: g++ -O1/-O2 assembly of an if/else using the macros is identical before and after; clang++ -O2 IR keeps
  the same `!prof` branch weights.
- Test: `attributes_test.cpp` static_asserts the type. Old: BUILD-FAIL. New: SUCCESS.
- Risks: code that deduced or bound `long` from the macro sees `bool`.

## Found but not fixed

| item | reason | decision |
|---|---|---|
| H4 `ATTR_ASSUME` no-op on GCC | impact low; the fix changes the macro's syntactic role on GCC (statement-only), tied to H5 | D2 |
| H5 `ATTR_ASSUME` expression vs statement | UNCLEAR locally (needs GCC >= 14 / Clang >= 19); owner question | D3 |
| H7 `ATTR_NO_UNIQUE_ADDRESS` layout depends on `-std` | API/ABI; owner decision | D4 |
| H8 `ATTR_TRIVIAL_ABI` ABI per compiler/std | documentation / ABI change; owner decision | D5 |
| H9 placement of declaration macros | documentation only, low | D6 |
| H13 config macros mixed across TUs | documented precondition; detection changes mangled names | D7 |
| H15 `__cxa_get_globals` path in gnu++11/14 | FORMAL, no observable effect; D1 | D8 |
| H17 `to_integer<volatile I>` | low; std-compatible signature; changing it is API | D9 |
| H18 Clang fold limit 256 | low; non-trivial rewrite | D10 |
| H19 `constexpr_for` depth ~446/507 | low; non-trivial rewrite; owner question | D11 |
| H21 `to_bytes`/`from_bytes` pointer arity trap | API or documentation; owner decision | D12 |
| H10, H11 (MSVC), H14 (Clang 22), H22 (C++26 lib), H23 (feature macros) | UNCLEAR, CI review probes | D13-D16 |
| O5 NDEBUG truncation of wide shift counts | documented precondition | D17 |

## Questions to the owner

- Q1. Accept 1c6c6b6 (`[[nodiscard]]` on GCC C++11/14)? It fixes the "discard with a cast" use and class types, but
  rejects the trailing placement `int f() ATTR_NODISCARD;` on GCC C++11/14 (already rejected in C++17).
- Q2. Accept 98c9497 (`unforward` returns a cv-unqualified value)?
- Q3. Accept 9fc253b (`ATTR_LIKELY`/`ATTR_UNLIKELY` yield `bool` everywhere)?
- Q4. `ATTR_ASSUME`: should it be a statement-only macro (then GCC can use `__attribute__((__assume__))` / `[[assume]]`
  gated on the attribute, H4), or must it stay usable as an expression (then the C++23 `[[assume]]` branch is wrong, H5)?
- Q5. `ATTR_NO_UNIQUE_ADDRESS`: enable it whenever the compiler supports the attribute (layout change for C++11-17 users),
  or document it as ABI-affecting across standards?
- Q6. `ATTR_TRIVIAL_ABI`: document that marked types must not cross compiler/standard boundaries? Drop the `>= C++11` gate
  on Clang?
- Q7. Should mismatched `STATE_SAVER_*` settings across TUs be made detectable (inline namespace / ABI tag per policy,
  `#pragma detect_mismatch` on MSVC)? It changes mangled names.
- Q8. `to_bytes`/`from_bytes` with pointer arguments: document, or exclude pointer types from the by-reference overloads?
- Q9. Do ranges above ~450 iterations for `constexpr_for`, or more than 256 arguments for `invoke_each`/`apply_each`
  (Clang), matter? If yes, both need a non-recursive / non-fold rewrite.
- Q10. If Clang >= 22 diagnoses `__COUNTER__` under `-pedantic` (H14): fall back to `__LINE__` in strict mode (which loses
  two guards per line), or document `-Wno-c2y-extensions` for users?

## Verification

Step 3 ran everything below on the final tree; the orchestrator repeated T1 (g++ and clang++ Debug, CMake + ctest: 26/26
each) on the review branch head after cherry-pick.

| # | what | configuration | result |
|---|---|---|---|
| 1 | CMake build + ctest + all 10 examples run | g++ Debug, clang++ Debug | ctest 26/26 each (24 existing + `attributes-cpp98.t` + `byte-gnu.t`), examples 10/10 rc=0 |
| 2 | same | g++ Release, clang++ Release (`-O3 -DNDEBUG`) | 26/26, examples 10/10 |
| 3 | same with sanitizers | g++ ASan+UBSan (`-fno-sanitize-recover=all`, leak detection on); clang++ UBSan trap | 26/26, examples 10/10 |
| 4 | direct matrix, every test source x every std, Debug | g++: c++11..c++23 + gnu++11..gnu++20; clang++: c++11..c++2c + gnu++11..gnu++20; C++98/03 attributes test; 7 state_saver configs + cxxabi-first at every std; byte/utility from c++17 | 273/273 PASS |
| 5 | same, Release `-O2 -DNDEBUG` | same | 273/273 PASS |
| 6 | same, sanitizers | g++ `-O1 -fsanitize=address,undefined`; clang++ `-O1 -fsanitize=undefined -fsanitize-trap=undefined` | 273/273 PASS, no `runtime error` |
| 7 | strict warnings on headers | 80 TUs, g++ c++11/17/23, clang++ c++11/17/2c | 80/80; header diagnostics unchanged from step 2 (O1 `-Wuseless-cast`, O2 `-Wreserved-identifier`) |
| 8 | all headers under `-fno-exceptions` / `-fno-rtti`, 3 state_saver policies | g++/clang++ c++11..c++23 | 90/90 |
| 9 | every attribute macro in one TU | g++ c++98..c++23, clang++ c++98..c++2c | 15/15 build (was 13/15) |
| 10 | fixes-only tree, and fixes + each proposal alone | g++/clang++ Debug CMake | ctest 26/26 in all 8 builds |
| 11 | old-vs-new regression runs per commit | see each commit | fail on base headers, pass on patched headers |

Not verified locally (environment limits): MSVC (x64/x86), ClangCL, AppleClang, libc++/libc++abi, `-m32`, GCC >= 14 /
Clang >= 19 / Clang 22, Clang ASan/LSan/MSan/libFuzzer, older GCC/Clang. The existing CI (ubuntu GCC 12-14 / Clang 16-18,
macOS AppleClang, Windows MSVC x64) runs the new targets and changed tests after push; CI review closes H5, H10, H11,
H14, H22, H23 and re-checks R1/R2/R4/R5.

## CI suggestions (workflow files not changed)

- The configurations where the fixed problems showed up are now covered by the test CMake itself (`attributes-cpp98.t`,
  `byte-gnu.t`, existing `attributes.t` / `attributes-cpp14.t` for GCC C++11/14).
- Minimal permanent extension to consider: one GCC >= 14 and one Clang >= 19 job building `attributes_test.cpp` at C++23
  (`[[assume]]` branch, H4/H5); one Clang >= 22 job (H14); a g++ ASan+UBSan job over ctest; a 32-bit job (MSVC Win32 or
  GCC -m32) and a libc++ job for the pre-C++17 `uncaught_exceptions()` path (R1).

## Decisions

Source: control run — decisions of previous reviews were deliberately NOT imported (user instruction); previous review
branches were not read. The only imported decision is D1, taken from the runbook "Applicability" section (summary of the
first review run of yacppl).

| ID | decision | why | reference |
|---|---|---|---|
| D1 | The pre-C++17 `nstd::detail::uncaught_exceptions()` implementation via `__cxa_get_globals()` + fixed offset is kept; its main risk (libc++abi and 32-bit layouts) is considered closed. | First review's CI review ran a runtime probe (R1, runbook Appendix C) on macos-appleclang (libc++abi), clang-libcxx and gcc-m32: all green. | runbook "Applicability" (yacppl, first run); `include/state_saver.hpp:104-107` |
| D2 | H4 (`ATTR_ASSUME` is `static_cast<void>(0)` on GCC in every mode, incl. GCC 13 `-std=c++23`) is left as is (deferred). | Confirmed, impact low. The available fix (`__attribute__((__assume__(e)))` on GCC >= 13 or gating `[[assume]]` on the attribute instead of `__cplusplus`) turns the macro into a statement-only form on GCC, i.e. it changes its syntactic role (H5) for existing users; not "trivial and obviously safe". Revisit together with the owner answer on H5 (report Q4) and CI probes `H4.cpp`/`H5.cpp`. | evidence.md C2, verdict H4; `include/attributes.hpp:119-129` (HEAD) |
| D3 | H5 (`ATTR_ASSUME` is an expression in some branches and a statement in the `[[assume]]` branch) is not changed; waits for CI review and the owner. | UNCLEAR locally: the `[[assume]]` branch is taken only by GCC >= 14 / Clang >= 19 at C++23, not available in the container. Owner question: document as statement-only or keep an expression form (report Q4). | verdict H5; CI probe `.review/probes/H5.cpp` |
| D4 | H7 (`ATTR_NO_UNIQUE_ADDRESS` empty below C++20 on GCC/Clang, so a type's layout depends on `-std`) is not changed; waits for the owner. | Confirmed, but every fix is API/ABI: enabling the attribute whenever `__has_cpp_attribute(no_unique_address)` changes the layout for current C++11-17 users; the alternative is documentation that the macro is ABI-affecting (report Q5). | verdict H7; `include/attributes.hpp:200-208` (HEAD) |
| D5 | H8 (`ATTR_TRIVIAL_ABI` changes the calling convention per compiler and, on Clang, between C++98 and C++11) is not changed; waits for the owner. | Cross-compiler mismatch is inherent to `trivial_abi` (documentation); dropping the `>= 201103L` gate changes the ABI of Clang C++98 users (API/ABI). Report Q6. | verdict H8; `include/attributes.hpp:188-194` (HEAD) |
| D6 | H9 (valid placement of declaration macros differs between standards) is left as is (deferred, documentation). | Confirmed, impact low, documentation-only fix ("place declaration macros before the declaration"); the runbook limits README edits to real necessity. Proposal 1c6c6b6 (if accepted) makes GCC C++11/14 follow the C++17 placement rule for `ATTR_NODISCARD`. | verdict H9, observation O7; README "Declaration attributes" |
| D7 | H13 (mixing state_saver configuration macros across TUs is a silent ODR violation) is kept as a documented precondition. | README and the header require the same settings in every TU; making violations detectable (inline namespace / ABI tag per policy, `#pragma detect_mismatch`) changes mangled names (API/ABI). Owner question (report Q7). | verdict H13, row 30 / O4; README "Configuration"; `include/state_saver.hpp:49` |
| D8 | H15 (pre-C++17 `__cxa_get_globals` path is used even in `gnu++11/14`, where `std::uncaught_exceptions` exists) is not changed. | FORMAL: no observable effect on libstdc++/LP64 (row 20: 0 mismatches in gnu++11/14); D1 keeps the fallback. Selecting the std path on `__cpp_lib_uncaught_exceptions` is an optional refinement for the owner. | verdict H15; D1; `include/state_saver.hpp:100-110` |
| D9 | H17, `nstd::to_integer<volatile I>` part (volatile-qualified return type, `-Wdeprecated-volatile` on Clang C++20) is left as is (deferred). | Impact low; `std::to_integer` has the same signature (libstdc++ only escapes the warning as a system header); returning `std::remove_cv_t<I>` changes the public signature. The `unforward` part is in proposal 98c9497. | verdict H17; `include/byte.hpp:91-93` |
| D10 | H18 (Clang fold-expression limit of 256 operands in `invoke_each`/`apply_each`) is left as is (deferred). | Confirmed, impact low (Clang only, > 256 arguments); the fix is a rewrite of the constraints and bodies without folds, not a trivial change. | verdict H18; `include/utility.hpp:47-80` |
| D11 | H19 (`constexpr_for` limited to ~446 (GCC) / 507 (Clang) iterations by template depth) is left as is (deferred). | Confirmed, impact low; a non-recursive implementation is a rewrite; owner question whether larger ranges matter (report Q9). | verdict H19, row 24 / O3; `include/utility.hpp:136-148,276-285` |
| D12 | H21 (`to_bytes`/`from_bytes` with a pointer argument copy the pointer or the pointee depending on arity) is not changed; waits for the owner. | Behaviour follows "copy the object representation"; options are documentation or excluding pointer types from the by-reference overloads (API). Report Q8. | verdict H21; `include/byte.hpp:146-188`; README byte section |
| D13 | H10, H11 (MSVC pre-C++17 `ATTR_NODISCARD` no-op / `__pragma(warning(suppress))` covers one line) are not changed. | UNCLEAR: MSVC only, not in the container; closed by CI review probes. | verdicts H10, H11; `.review/probes/H10.cpp`, `H11.cpp` |
| D14 | H14 (`__COUNTER__` in guard macros vs Clang >= 22 `-pedantic-errors`) is not changed. | UNCLEAR: needs Clang >= 22 (not in the container or CI); the fix direction is an owner decision (report Q10). | verdict H14; `.review/probes/H14.cpp`; `include/state_saver.hpp:307-313` |
| D15 | H22 (`nstd::Trivial` uses `std::is_trivial`, deprecated in C++26) is not changed. | UNCLEAR: needs a C++26 standard library that deprecates `is_trivial`; closed by CI review probe. | verdict H22; `.review/probes/H22.cpp` |
| D16 | H23 (`_v` traits / constexpr `nstd::unused` gated on feature-test macros) is not changed. | UNCLEAR: refuted for g++ 13 / clang++ 18; MSVC/AppleClang via CI review probe. | verdict H23; `.review/probes/H23.cpp` |
| D17 | O5 (`nstd::byte` shift by a count wider than `unsigned int` is truncated under `NDEBUG`) is kept. | Documented precondition (README: count smaller than the bit width of `unsigned int`); Debug builds assert, also for `__int128` counts after commit 1190716. | evidence.md O5; `include/byte.hpp:113-123` (HEAD) |

## Hypothesis verdicts

Step 2b. Revision `c34df7597731193d163a86bf526f45ee1bafc94a`; repository code not modified. Probes ran against the real
headers (`-I /home/user/yacppl/include`) in `/tmp/yacppl-step2b/local/` (sources copied from `/tmp/yacppl-step1/`, corrected
where noted); one log per hypothesis in `/tmp/yacppl-step2b/logs/hNN.log`. `W` = `-Wall -Wextra -pedantic-errors -Werror`
(project set). g++ 13.3.0 and clang++ 18.1.3, libstdc++ 13. GCC middle-end diagnostics need a real compile, so H1/H2/H6
use `-c` (O9).

| H | verdict | command | key output line | comment |
|---|---|---|---|---|
| H1 | CONFIRMED | `for c in g++ clang++; for s in c++11 c++14 c++17 c++20: $c -std=$s W -I include -c -o /dev/null nodiscard3.cpp` (`static_cast<void>(f())` on an `ATTR_NODISCARD` function) | g++ c++11/c++14: `error: ignoring return value of 'int f()' declared with attribute 'warn_unused_result' [-Werror=unused-result]`; g++ c++17/20 and clang++ all: rc=0 | `(void)f()` fails the same way on g++ c++11. With `-fsyntax-only` g++ c++11 is silent (rc=0), so the bug is invisible to syntax-only checks. Boundaries: GCC, pre-C++17 only. |
| H2 | CONFIRMED | `$c -std={c++98,c++11,c++14,c++17} W -I include -c nodiscard_class.cpp` (`struct ATTR_NODISCARD S`, result used) | g++ c++98/11/14: `error: 'warn_unused_result' attribute only applies to function types [-Werror=attributes]`; g++ c++17, clang++ c++11..17: rc=0 | Same as C3. Refinement accepted from step 2: it is the `-Wattributes` warning (`g++ -std=c++14 -Wall -Wextra`: `warning: ... [-Wattributes]`, rc=0), an error only with `-Werror`. Not a hard error. |
| H3 | CONFIRMED | `$c -std={c++11,c++20} W -I include likely_type.cpp && ./a.out`; `likely_obs.cpp` (`auto hint = ATTR_LIKELY(argc > 0); std::cout << std::boolalpha << hint`) with the project macro vs the non-GNU fallback forced by `-D'ATTR_LIKELY(x)=(static_cast<bool>(x))'` | `is_same<decltype(ATTR_LIKELY(c)), bool>=0 is_same<..., long>=1` (g++ and clang++, both std); output `1` (project macro) vs `true` (fallback definition) | The type is `long` on GCC/Clang and `bool` on every other compiler (fallback text, attributes.hpp:159,168). Observable through `auto`, `decltype`, overloads and stream output. API change if fixed. |
| H4 | CONFIRMED | `g++ -std={c++11,c++17,c++20,c++23} -E -P -I include` on `ATTR_ASSUME(x >= 0)`; `g++ -std=c++23 -O2 -S` instruction count, macro vs `[[assume]]` vs `__attribute__((__assume__))` at c++11 | every std: `int f(int x){ static_cast<void>(0); return x / 2; }`; c++23: `__cplusplus=202100L`, `__has_cpp_attribute(assume)=202207`, `__has_attribute(assume)=1`; 5 instructions with the macro, 3 with `[[assume]]` (c++23) and 3 with `__attribute__((__assume__))` (c++11, W clean) | Same as C2. GCC >= 14 at c++23 (`__cplusplus` = 202302L) is expected to take the `[[assume]]` branch: CI probe `H4.cpp` (gcc-latest) confirms the no-op at c++17/c++20 there and checks the c++23 branch. |
| H5 | UNCLEAR | `$c -std={c++17,c++23} W -I include assume_expr.cpp` (`return (ATTR_ASSUME(x > 0), x);`); branch text emulated with `-D'ATTR_ASSUME(e)=[[assume(e)]]'` | project macro: rc=0 on g++ and clang++ at both std (branch never taken: g++ 13 `__cplusplus`=202100L, clang++ 18 `__has_cpp_attribute(assume)`=0); emulated branch, g++ c++23: `error: expected identifier before '[' token` | The real `[[assume]]` branch needs GCC >= 14 or Clang >= 19, not in the container. CI probe `H5.cpp` (gcc-latest, clang-libcxx, macos-appleclang, msvc-x64, clangcl). |
| H6 | CONFIRMED | `$c -std={c++98,c++03,c++11,c++17} W -Wimplicit-fallthrough -I include -c fall.cpp`; control `fall2.cpp` with `__attribute__((__fallthrough__));` | g++ c++98/03: `error: this statement may fall through [-Werror=implicit-fallthrough=]` (also with the project flags alone); clang++ c++98/03: `error: unannotated fall-through between switch labels [-Werror,-Wimplicit-fallthrough]`; c++11/17 rc=0; control rc=0 in every row | Same as C1. The GNU attribute works in C++98 on both compilers, so the empty fallback is avoidable. |
| H7 | CONFIRMED | `$c -std=c++17 W -I include -c l17.cpp && $c -std=c++20 W -I include l20.cpp l17.o && ./a.out` (layout.hpp: `ATTR_NO_UNIQUE_ADDRESS empty_tag tag; int value;`) | `c++17 TU: 8, c++20 TU: 4`, rc=1 (g++ and clang++); plain `[[no_unique_address]]` at c++11 W: rc=0 on both, `sizeof(h) == sizeof(int)` | One header type, two layouts in one program. Owner question (enable the attribute where supported = layout change for C++11-17 users, or document as ABI-affecting). |
| H8 | CONFIRMED | `tabi_def.cpp` / `tabi_main.cpp` (`struct ATTR_TRIVIAL_ABI handle` with user copy ctor and dtor, `int read_handle(handle)`), `-O1`, definer/caller pairs, linked with g++ | def clang++ c++11 / main g++ c++11: `read_handle -> 1234160052 (expected 42)` rc=1; def g++ / main clang++: SIGSEGV rc=139; def clang++ c++98 / main clang++ c++11: SIGSEGV rc=139; clang/clang c++11 and g++/g++: 42, rc=0; `__attribute__((trivial_abi))` in clang++ c++98 W: rc=0 | Cross-compiler mismatch is inherent to `trivial_abi` (documentation); the clang c++98 vs c++11 mismatch comes from the macro's `>= 201103L` gate (attributes.hpp:175) and is avoidable. |
| H9 | CONFIRMED | `$c -std={c++11,c++14,c++17,c++20} W -I include -c placement.cpp` (`int old_api() ATTR_DEPRECATED("..."); int checked() ATTR_NODISCARD;`) | g++ c++11/14 rc=0, g++ c++17/20: `attributes.hpp:123:40: error: 'nodiscard' attribute can only be applied to functions or to class or enumeration types [-Werror=attributes]`; clang++ c++11 rc=0, c++14/17/20: `error: 'deprecated' attribute cannot be applied to types` | Code valid at one standard breaks when the standard is raised. Documentation fix (placement before the declaration). See also O7 (`ATTR_MAYBE_UNUSED`). |
| H10 | UNCLEAR | needs MSVC (`/std:c++14` vs `/std:c++17`) | - | No MSVC in the container. CI probe `H10.cpp` (msvc-x64, msvc-x86). ClangCL dropped from the step 1 draft: it takes the `__clang__` branch, the c++14 row would not hold for a reason unrelated to H10. |
| H11 | UNCLEAR | needs MSVC before `/std:c++17` | - | CI probe `H11.cpp` (msvc-x64, msvc-x86). Corrected the draft: `expect` is now the hypothesis's prediction (c++14 `build-fail "warning C41"`, c++17 `build-ok` as control); ClangCL dropped (takes the `__clang__` branch). |
| H12 | CONFIRMED | `$c -std={c++11,c++17,c++20} W -I include saver_rref.cpp` (SAVER_EXIT on `std::string&&` parameter and on forwarding `T&&` bound to an rvalue), `saver_rref2.cpp` (`WITH_SAVER_FAIL(v)`, `MAKE_SAVER_SUCCESS(guard, v)`), control `saver_rref_ctrl.cpp` (`nstd::saver_exit<std::string&> guard{s}`) | all 12 macro rows: `state_saver.hpp:178: error: static assertion failed: state_saver requires lvalue type.` (clang: `'!std::is_rvalue_reference<int &&>::value'`); control: build-ok, run rc=0 on every compiler/std | The macros reject named rvalue-reference variables that are lvalues; the class itself accepts them. |
| H13 | CONFIRMED | `cd odr; $c -std=c++11 {-O0,-O2} -I include a.cpp b.cpp` and `b.cpp a.cpp` (TU a SUPPRESS, TU b default, same `saver_exit<thrower&>`); `g++ -O2 -flto -Wodr` | -O0 `a.cpp b.cpp`: `tu_b: exception swallowed (TU A's destructor was linked)` rc=1; -O0 `b.cpp a.cpp`: `tu_a: exception escaped a SUPPRESS guard` rc=2 (g++ and clang++); -O2: rc=0 both orders; LTO: no `-Wodr` diagnostic, rc=1 | Observable and silent, but the README requires equal settings in every TU (documented precondition, same as row 30 / O4). Owner question whether to make violations detectable. |
| H14 | UNCLEAR | needs Clang >= 22 | g++ 13 / clang++ 18 at c++11 and c++17: build-ok (runner dry run) | The diagnostic exists only from Clang 22 (`-Wc2y-extensions`, already suppressed for the project's own targets). CI probe `H14.cpp` (clang-libcxx; meaningful only if that job runs Clang >= 22). |
| H15 | FORMAL | `h15_lib.cpp` (`#ifdef __cpp_lib_uncaught_exceptions return std::uncaught_exceptions();`) and `$c -std=$s -E -P -I include` on `nstd::detail::uncaught_exceptions()`, `s` in c++11, c++14, gnu++11, gnu++14 | gnu++11/gnu++14: `std::uncaught_exceptions` usable rc=0 and the TU still contains the `__cxa_get_globals()) + sizeof(void*)` body (count 1), g++ and clang++; c++11/c++14: `#error __cpp_lib_uncaught_exceptions not defined` | The layout-dependent fallback is selected although the std function is available in GNU dialects, but on libstdc++/LP64 it returns the same counts (row 20: gnu++11/gnu++14, 43 checks, 0 mismatches; R1.cpp below under gnu++14 + sanitizers: ok). No observable effect here; any effect would be the R1 layout risk, which the CI probes R1/R1-cxxabi re-check. |
| H16 | CONFIRMED | `$c -std={c++17,c++20} W -I include` on `unforward.cpp` (static_assert on the type), `unforward2.cpp` (copy/move counters), `unforward3.cpp` (type with deleted copy assignment) | `static assertion failed: unforward of a const lvalue should materialize a non-const value`; `copy_assigns=1 move_assigns=1` rc=1; `unforward3.cpp:12: error: use of deleted function 'move_only_assign& move_only_assign::operator=(const move_only_assign&)'` (clang: `overload resolution selected deleted operator '='`) while `decay_copy` on line 11 compiles | `unforward` of a const class lvalue returns a `const T` prvalue: cannot be moved from. API change if fixed; owner question. |
| H17 | CONFIRMED | `$c -std={c++17,c++20,c++23} W -I include -c volret.cpp` (`nstd::unforward(volatile int&)`, `nstd::to_integer<volatile int>`), control `volret_std.cpp` (`std::to_integer<volatile int>(std::byte{2})`) | clang++ c++20/c++23: `utility.hpp:192:25: error: volatile-qualified return type ... is deprecated [-Werror,-Wdeprecated-volatile]` and `byte.hpp:92:25: error: ...`; clang++ c++17 and all g++ rows: rc=0; std control: rc=0 everywhere | Correction to H17's "std::to_integer has the same issue": with libstdc++ the std version builds clean (system header), so the nstd version is stricter than std. Clang only. |
| H18 | CONFIRMED | `timeout 120 $c -std=c++17 W -I include -fsyntax-only` on `apply_256.cpp`, `apply_257.cpp`, `apply_1000.cpp`, `invoke_300.cpp`, control `apply_std_1000.cpp` (`std::apply` with a braced pack expansion) | clang++: `apply_257`/`apply_1000`: `utility.hpp:53:150: fatal error: instantiating fold expression with 257 (1000) arguments exceeded expression nesting limit of 256`; `invoke_300`: `utility.hpp:47:87: fatal error: ... 300 arguments ...`; `apply_256` and `apply_std_1000` rc=0; g++: all rc=0 | Clang only (`-fbracket-depth` default 256). |
| H19 | CONFIRMED | `timeout 120 $c -std=c++17 W -I include -fsyntax-only cfor_N.cpp` (`constexpr_for<0, N, 1>` in a constexpr and a runtime function), N = 446, 447, 448, 507, 508, 1000 | g++: N=446 build+run rc=0, N=447: `fatal error: template instantiation depth exceeds maximum of 900`; clang++: N=507 rc=0, N=508: `recursive template instantiation exceeded maximum depth of 1024` | Limits match the hypothesis (446 / 507); row 24 reports 447 for g++ with its own probe, the difference is the extra function in this probe. Undocumented compile-time limit; owner question. |
| H20 | CONFIRMED | `$c -std={gnu++17,gnu++20,c++17} W -I include -fsyntax-only shift128.cpp`; `shift128_run.cpp` (`-std=gnu++17 -Wall -Wextra -Werror`, asserts on): `nstd::byte{1} << ((u128)1 << 64) + 1` | gnu++17/20, g++ and clang++: `static assertion failed: wide extended-integer shift counts must not be truncated before validation`; run: `is_valid_byte_shift(2^64+1) = 1`, `byte{1} << (2^64+1) = 2`, rc=0 (assert did not fire); c++17: `unsigned __int128` is not integral (precondition static_assert) | GNU dialects only (`__int128` integral). The documented precondition check is bypassed and the shift silently uses count 1. |
| H21 | CONFIRMED | `$c -std=c++17 W -I include ptrtrap.cpp && ./a.out` | `q==p: 1, target=0`, rc=1, no diagnostics (g++ and clang++) | Behaviour follows "copy the object representation", but the same argument means the pointer or the pointee depending on arity; README (lines 59-60) does not mention it. Owner question / documentation. |
| H22 | UNCLEAR | needs a C++26 standard library that deprecates `std::is_trivial` (libstdc++ 15+, libc++ 21+, MS STL) | g++ 13: `-std=c++2c` not supported; clang++ 18 c++2c with libstdc++ 13: build-ok (no deprecation marker) | CI probe `H22.cpp` (gcc-latest, clang-libcxx, macos-appleclang, msvc-x64). |
| H23 | UNCLEAR | runner dry run of `H23.cpp` with g++ and clang++ at c++14/17/20 | build-ok on both (feature-test macros defined, `_v` traits and constexpr `nstd::unused` present) | Not affected on GCC 13 / Clang 18. MSVC, ClangCL and AppleClang: CI probe `H23.cpp`. `expect` corrected to the hypothesis's prediction (`build-fail`); build-ok there refutes H23. |

Counts: CONFIRMED 16 (H1, H2, H3, H4, H6, H7, H8, H9, H12, H13, H16, H17, H18, H19, H20, H21), REJECTED 0, FORMAL 1 (H15),
UNCLEAR 6 (H5, H10, H11, H14, H22, H23).

### CI probes

Files in `.review/probes/`, runner format of Appendix A. Every probe was dry-run with the unchanged runner
(`/tmp/yacppl-step2b/review/probes/run.py --job <id> --style gcc --cxx g++|clang++`, include/ copied to `/tmp/yacppl-step2b/include/`,
logs `/tmp/yacppl-step2b/logs/run-*.log`): all probes build with `-Wall -Wextra -pedantic-errors -Werror` on g++ 13 and clang++ 18
wherever a local outcome is expected. Rows for MSVC-only predictions (H10, H11, H23, R4, R5 on Windows) were run with g++/clang++ only as a
sanity check of the source; their predictions are meaningful only on the listed jobs.

| probe | where | std | expect | local dry run |
|---|---|---|---|---|
| `H4.cpp` | gcc-latest | c++17 c++20 c++23 | build-fail "ATTR_ASSUME is a no-op"; c++23: build-ok | g++ 13: held at c++17/20, NOT held at c++23 (GCC 13 never takes `[[assume]]`, as the meaning line says) |
| `H5.cpp` | gcc-latest clang-libcxx macos-appleclang msvc-x64 clangcl | c++17 c++23 | build-ok; c++23: build-fail | c++23 build-ok locally (branch not taken) |
| `H10.cpp` | msvc-x64 msvc-x86 | c++14 c++17 | c++14: build-ok; c++17: build-fail "C4834" | MSVC only |
| `H11.cpp` | msvc-x64 msvc-x86 | c++14 c++17 | c++14: build-fail "warning C41"; c++17: build-ok | MSVC only; builds clean on g++/clang++ |
| `H14.cpp` | clang-libcxx | c++11 c++17 | build-fail "c2y-extensions" | build-ok on clang++ 18 (expected below Clang 22) |
| `H22.cpp` | gcc-latest clang-libcxx macos-appleclang msvc-x64 | c++20 c++2c | build-ok; c++2c: build-fail "deprecated" | clang++ 18 + libstdc++ 13: c++2c build-ok; g++ 13: c++2c skipped (std not supported) |
| `H23.cpp` | msvc-x64 msvc-x86 clangcl macos-appleclang | c++14 c++17 c++20 | build-fail | build-ok on g++/clang++ (refuted for them) |
| `R1.cpp` | macos-appleclang clang-libcxx gcc-latest gcc-m32 | c++11 c++14 c++17 | run-ok | held on g++ and clang++ (64-bit, libstdc++), also gnu++14 with ASan/UBSan; mutation check: offset `sizeof(void*) + sizeof(int)` in a copy of the header -> c++14 `check_on_unwind: expected 1, got 0` ... `failed` rc=1, c++17 control ok (`logs/r1-mutant.log`) |
| `R1-cxxabi.cpp` | macos-appleclang clang-libcxx gcc-latest gcc-m32 | c++11 c++14 | run-ok | held (libstdc++ `<cxxabi.h>` first) |
| `R2.cpp` | msvc-x64 msvc-x86 clangcl macos-appleclang clang-libcxx | c++14 c++17 c++20 | run-ok | held on g++/clang++ |
| `R4.cpp` | msvc-x64 msvc-x86 clangcl | c++14 c++17 c++20 | run-ok | held on g++/clang++ (`sizeof(holder)` 8 -> 4 at c++20) |
| `R5.cpp` | msvc-x64 msvc-x86 clangcl gcc-m32 | c++17 c++20 | run-ok | held on g++/clang++ 64-bit (`-m32` unavailable locally) |

Profile risks without a probe: R3 (container, row 30 / H13), R6 (container; documented and pinned by tests, step 1), R7 (older
compilers are not CI review jobs). R2 for old macOS deployment targets cannot be expressed by a probe (no per-probe flags such as
`-mmacosx-version-min`); `R2.cpp` covers the default target and the MSVC/ClangCL pre-C++17 branch.

## Empirical checks (step 2)

| # | check | configuration | result | command |
|---|---|---|---|---|
| 1 | T1 baseline: CMake build + ctest | g++ 13, Debug, Ninja | PASS: build OK (incl. 7 standalone header OBJECT targets at min std: attributes `-std=c++98`, state_saver `-std=c++11`, byte `-std=c++17`, verified in build.ninja; both `try_compile` checks), ctest 24/24 | `cmake -S . -B build-gcc-debug -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug && cmake --build ... && ctest ...`; `logs/t1-gcc-debug-{configure,build,ctest}.log` |
| 2 | T1 baseline: CMake build + ctest | clang++ 18, Debug | PASS: ctest 24/24 | same with clang++; `logs/t1-clang-debug-*.log` |
| 3 | T1/T2/P8: every test source on every standard, Debug (`-O0 -g`) | g++: c++11,14,17,20,23 (71 jobs); clang++: c++11..c++2c (86 jobs). attributes/unused/concepts/type_traits/state_saver (+`STATE_SAVER_TEST_CXXABI_FIRST`) + 7 configs at every std; byte/utility from c++17 | PASS 157/157 | `MODE=debug python3 matrix.py std-debug jobs_std.py`; `logs/std-debug/SUMMARY.tsv` + one log per job |
| 4 | T2: same matrix, Release | `-O2 -DNDEBUG`, both compilers | PASS 157/157 | `MODE=release python3 matrix.py std-release jobs_std.py`; `logs/std-release/SUMMARY.tsv` |
| 5 | T2: CMake Release build + ctest | g++ / clang++, `CMAKE_BUILD_TYPE=Release` (-O3 -DNDEBUG) | PASS 24/24 each | `logs/t2-{gcc,clang}-release-*.log` |
| 6 | T2: examples build + run | 10 examples x {g++, clang++} x {Debug, Release} | PASS: 40/40 runs rc=0 | `logs/example-{gcc,clang}-{debug,release}-*.log` |
| 7 | T1 sanitizers over the full std matrix | g++: `-O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all` (+`detect_leaks=1`), 71 jobs; clang++: `-O1 -g -fsanitize=undefined -fsanitize-trap=undefined`, 86 jobs | PASS 157/157, no `runtime error` in any log | `python3 matrix.py san jobs_san.py`; `logs/san/SUMMARY.tsv` |
| 8 | T1 sanitizers via CMake (tests + examples) | g++ ASan+UBSan Debug; clang++ UBSan-trap Debug | PASS: ctest 24/24 each; 20/20 example runs rc=0 (LeakSanitizer on for g++) | `logs/san-cmake-{gcc,clang}-*.log`, `logs/example-{gcc-asan,clang-ubsan}-*.log` |
| 9 | T2 (for H15): GNU dialects | `-std=gnu++11/14/17/20`, `-O2 -DNDEBUG`, both compilers, same test set | PASS 112/112 | `python3 matrix.py gnu jobs_gnu.py`; `logs/gnu/SUMMARY.tsv` |
| 10 | Item 2: unmask suppressions | `/wd4702`, MSVC `__pragma(warning(suppress ...))` (attributes.hpp:149, state_saver.hpp:295) | NOT RUNNABLE: MSVC-only, no MSVC in container -> CI only | - |
| 11 | Item 2: unmask `-Wno-c2y-extensions` (Clang >= 22 only) | clang++ 18 | NOT RUNNABLE: clang 18 does not know the group (`warning: unknown warning option '-Wc2y-extensions'`); clang 18 `-Weverything -pedantic` gives no diagnostic for `__COUNTER__` -> what the suppression hides is CI only (Clang 22) | `clang++ -Wc2y-extensions -fsyntax-only -x c++ /dev/null`; `clang++ -std=c++11 -pedantic -Weverything -fsyntax-only` on `int a = __COUNTER__;` |
| 12 | Item 3: strict warnings, project headers only (doctest via `-isystem`) | g++: `-Wall -Wextra -pedantic -Wconversion -Wsign-conversion -Wshadow -Wold-style-cast -Wuseless-cast -Wcast-qual -Wzero-as-null-pointer-constant -Wextra-semi` at c++11/17/23; clang++: `-Weverything` minus compat/padded/unsafe-buffer groups at c++11/17/2c; 80 TUs (all tests + 7 configs) | Headers: only 2 diagnostics: g++ `-Wuseless-cast` at attributes.hpp:158,167 (`ATTR_LIKELY/UNLIKELY` of a bool argument, reported in the user TU); clang `-Wreserved-identifier` at state_saver.hpp:67,68 (`__cxa_eh_globals`, `__cxa_get_globals`, c++11 only). Test files: `-Wshadow` 195, `-Wunused-member-function` 114, `-Wunneeded-member-function` 83, `-Wshadow-field-in-constructor` 65, `-Wmissing-prototypes` 15, `-Wunused-template` 14, `-Wmissing-noreturn` 9 | `python3 matrix.py strict jobs_strict.py`; `grep -E "^/home/user/yacppl/include/.*warning" logs/strict/*.log` |
| 13 | Item 5: newest compilers, `-pedantic-errors` at each module's min std | standalone headers (attributes c++98, unused/concepts/type_traits/state_saver c++11, byte/utility c++17) + all tests at min std | PASS (rows 1-3) | build.ninja FLAGS: `-g -std=c++98 -Wall -Wextra -pedantic-errors -Werror` for attributes-standalone |
| 14 | Item 6: attribute macro expansion per compiler/std | g++ c++98..c++23, clang++ c++98..c++2c | Table in log. Empty/no-op expansions: `ATTR_FALLTHROUGH` empty in c++98/03 (both); `ATTR_ASSUME` = `static_cast<void>(0)` on g++ in every mode incl. c++23; `ATTR_TRIVIAL_ABI` empty on g++ (by design) and clang c++98/03; `ATTR_NO_UNIQUE_ADDRESS` empty below c++20 (by design) | `g++/clang++ -std=$s -E -P -I include probes/attr_expand.cpp`; `logs/attr-expand.log` |
| 15 | Item 6: attribute behaviour (diagnostic actually produced / suppressed) | NODISCARD, NODISCARD on class, NODISCARD_MSG, MAYBE_UNUSED, FALLTHROUGH, NORETURN, DEPRECATED, TRIVIAL_ABI; full compile (`-c`, GCC emits `warn_unused_result` and `-Wimplicit-fallthrough` only after the front end) with `-Wall -Wextra -pedantic -Wimplicit-fallthrough -Wreturn-type` | DEPRECATED warns with message everywhere; NODISCARD warns everywhere; NODISCARD_MSG shows the message from c++20; MAYBE_UNUSED / NORETURN silence their warnings everywhere; TRIVIAL_ABI: clang c++11+ `static_assert(__is_trivially_relocatable(P))` holds. FAIL: FALLTHROUGH in c++98/03 (both compilers warn, see C1); NODISCARD on a class on g++ c++98..c++14 gives `-Wattributes` (see C3) | `probes/attr_behave.cpp -DT_<name>`; `logs/attr-behave2.log` (`logs/attr-behave.log` is the first run with `-fsyntax-only`, where GCC middle-end warnings are absent; do not use it for GCC) |
| 16 | Item 6: ALWAYS_INLINE / ASSUME / NO_UNIQUE_ADDRESS / LIKELY by codegen | `-O0 -S` (inline), `-O2 -S` (assume: instruction count of `x/2` with vs without), `sizeof` | ALWAYS_INLINE: inlined at -O0 everywhere. NO_UNIQUE_ADDRESS: `sizeof(S)` 8 -> 4 from c++20 on both. LIKELY/UNLIKELY: build OK everywhere. ASSUME: clang 3 vs 5 instructions (effective, all std); g++ 5 vs 5 (no effect, all std) -> C2 | same probe, `-DT_ALWAYS_INLINE/T_ASSUME/T_NUA/T_LIKELY`; `logs/attr-behave.log` rows "ALWAYS_INLINE(-O0)", "ASSUME insns", "NO_UNIQUE_ADDRESS sizeof(S)" |
| 17 | Item 6/5: all attribute macros in one TU, `-Wall -Wextra -pedantic-errors -Werror` | g++ c++98..c++23, clang++ c++98..c++2c | 13/15 PASS; FAIL g++ c++98 and c++03: `error: this statement may fall through [-Werror=implicit-fallthrough=]` (C1) | `probes/attr_all98.cpp`; `logs/attr-all-<cxx>-<std>.log` |
| 18 | P2: CMake compile-fail test reason | `state_saver_destructor_compile_test.cpp` c++11, both compilers | PASS: positive control compiles; negative fails with exactly one error, the intended `static assertion failed: state_saver requires nothrow destructible type.` (state_saver.hpp:188) | `logs/p2-destructor-{g++,clang++}.log` |
| 19 | P2: diagnostic reason of 29 negative cases (state_saver static_asserts and `#error`s, byte/utility SFINAE) + 2 positive controls | both compilers | 58/58 reject/accept as intended; 56 match the expected text, 2 (clang `byte << bool`, `byte << 1.0`) are rejected with the equivalent `invalid operands to binary expression`. The first diagnostic is always the intended one; g++ adds cascades (e.g. `saver_exit<const int>`: 5 errors, first `requires not const type`) | `python3 probes/p2_cases.py`; `logs/p2-cases.log` |
| 20 | P3/R1: `nstd::detail::uncaught_exceptions()` vs `std::uncaught_exceptions()` at nested unwinding depth 0..6, plus SAVER_FAIL/SUCCESS inside a destructor during unwinding | both compilers, `gnu++11`, `gnu++14` (the `__cxa_get_globals` path), c++17, c++20; -O0/-O2 | PASS: 16/16 configs, 43 checks each, 0 mismatches; libstdc++/LP64 only (libc++abi, 32-bit: CI, D1) | `probes/uncaught_probe.cpp`; `logs/p3-uncaught.log` |
| 21 | P3: `__COUNTER__` unique guard names (several guards on one line, nested `WITH_SAVER_EXIT`, `-Wshadow`) | g++ c++11/17/23, clang++ c++11/17/23/2c | PASS 7/7. With the `__LINE__` fallback emulated (`-DNEARGYE_STATE_SAVER_COUNTER=__LINE__`) two guards on one line fail to compile (`redeclaration of ... SAVER_EXIT_2`) - observation O6 | `probes/p3_counter.cpp`; `logs/p3-counter.log` |
| 22 | Item 7: nstd vs std, `static_assert`/`decltype` harness + runtime differential | g++ c++23, clang++ c++2c; forward_like (vs P2445 reference, 30 combos), move/forward/move_if_noexcept, to_underlying, is_detected* (vs `std::experimental`), remove_cvref_t/type_identity_t, conjunction/disjunction (incl. short-circuit with incomplete type), negation, is_nothrow_convertible (16 pairs), byte ops vs std::byte (constexpr), cmp_* / in_range for all 10x10 standard integer type pairs at 8 boundary values (constexpr), runtime: bit_cast vs std::bit_cast, to_bytes/from_bytes vs memcpy, random cmp/in_range | PASS: all static_asserts hold; runtime 1 600 000 checks, 0 mismatches on both compilers | `probes/std_compare.cpp`; `logs/item7-{g++,clang++}.log` |
| 23 | Item 7: `is_nothrow_convertible` backport (pre-C++20 branch) vs `std::is_nothrow_convertible` | 33 type pairs; backport at c++11/14/17 diffed against c++20 output, both compilers | PASS: identical tables (6 diffs, all empty) | `probes/ntc_table.cpp`; `logs/ntc-<cxx>-<std>.txt` |
| 24 | Item 8: limits of `constexpr_for` (constexpr context, `-std=c++17`) | default `-ftemplate-depth`; binary search | g++: max N = 447 (N=448: `template instantiation depth exceeds maximum of 900`); clang++: max N = 507; a naive `if constexpr` recursion reaches 509 on both. N=400: g++ 0.6 s / 163 MB, clang 0.8 s / 130 MB. N=2000 with `-ftemplate-depth=5000`: g++ fails on `constexpr evaluation depth exceeds maximum of 512` (17.4 s, 1.95 GB); clang++ crashes with SIGSEGV (rc -11, no output) | `probes/cfor_n.cpp`, `probes/cfor_naive.cpp`; `logs/limits-cfor.log`, `logs/limits-cfor-<cxx>-<N>.log` |
| 25 | Item 8: extreme ranges of `constexpr_for` | `<INT_MAX-2, INT_MAX, 1>`, `<INT_MAX-5, INT_MAX, 4>`, `<0u, UINT_MAX, UINT_MAX>`, `<-5, 5, 3>`, `<5, 5, 1>`, `<LLONG_MIN, LLONG_MIN+2, 1LL>`, `unsigned char 250..255 step 10` | PASS: total iteration count 12 as expected (static_assert), no overflow diagnostics, both compilers | same probe |
| 26 | P5: boundary table of byte copy helpers + random overlap differential vs memmove (200 000 copies) | g++ ASan+UBSan and clang++ UBSan-trap, each with and without NDEBUG | PASS: main table 10/10 (debug) and 13/13 (NDEBUG) ok; Debug: null dst, overflowing count, `from_bytes<int>(nullptr)`, shift 32, shift by `1LL<<32`, shift -1 all abort on the intended `assert` (rc 134). NDEBUG: null/overflow cases are no-ops / return `T{}` as documented; shift 32 and shift -1 are UB caught by UBSan (documented precondition); shift by `1LL<<32` silently becomes shift 0 (O5) | `probes/p5_bytes.cpp [-DCASE=1..6]`; `logs/p5-summary.log`, `logs/p5/*.log` |
| 27 | P6: `-fno-exceptions` / `-fno-rtti` on the test sources | both compilers, c++11/17, `-fno-rtti`, `-fno-exceptions`, both | 104/132 build+run; all 28 failures are in test code or doctest (`cannot use 'throw' with exceptions disabled`, doctest `Exceptions are disabled!`); 0 errors located in `include/` | `python3 matrix.py noexc jobs_noexc.py`; `logs/noexc/SUMMARY.tsv` |
| 28 | P6: all headers used under `-fno-exceptions` / `-fno-rtti` | probe using every header, state_saver default / SUPPRESS / NO_THROW policy, c++11..c++23, 3 flag sets, both compilers | PASS 90/90 (build + run rc=0) | `probes/noexc_probe.cpp`; `logs/noexc-probe-summary.log` |
| 29 | P6: NDEBUG vs asserts (byte), all 7 state_saver config macros at every std | rows 3, 4, 7 | PASS | - |
| 30 | R3: mixed state_saver configuration across TUs | TU A default, TU B `STATE_SAVER_FORCE_COPY_ASSIGNABLE`, same `saver_exit<Tr>` | Links silently; behaviour depends on -O: -O0 both TUs move-assign (TU B expects copy), -O2 each TU gets its own. `g++ -flto -Wodr`: no diagnostic. Documented requirement (README "use the same values in every translation unit") - observation O4 | `probes/odr_a.cpp probes/odr_b.cpp`; `logs/r3-odr.log` |
| 31 | P7: `<windows.h>`/`<rpcndr.h>` emulation before all headers | `min`/`max` function macros, `small`, `interface`, `near`, `far`, `IN`, `OUT`, `CONST`, `DELETE`, `ERROR`, `TRUE/FALSE`, `typedef unsigned char byte`; c++11/17/20/23, both compilers | PASS 8/8 (build + run). Real `windows.h`: CI only | `probes/p7_winmacros.cpp`; `logs/p7-summary.log` |
| 32 | P8: newest standards / feature branches | covered by rows 3, 4, 7, 9, 14 (c++23 g++, c++2c clang) | PASS. Branches that cannot be reached locally: `[[assume]]` (needs `__cplusplus >= 202302L` and `__has_cpp_attribute(assume)`: g++ 13 has the attribute but `__cplusplus` = 202100L, clang 18 has `__cplusplus` = 202302L but no attribute) -> CI (gcc-14) | `g++ -std=c++23 -dM -E` |
| 33 | P1 (partial, profile): `add_subdirectory` consumer | consumer project, `yacppl::yacppl`, c++17, both compilers | PASS 2/2; `YACPPL_OPT_BUILD_TESTS/EXAMPLES` default OFF for a subproject. `cmake --install`/`find_package`: not applicable (no install rules) | `/tmp/yacppl-step2/consumer`; `logs/p1-addsub-{g++,clang++}.log` |

Totals: 33 rows; 25 PASS, 3 with confirmed problems (rows 15, 16, 17), 3 not runnable here (rows 10, 11; part of 32),
2 with expected/test-side failures (row 27 test code, row 30 documented ODR). Across all matrices: 157+157+157+112+90
= 673 build+run jobs, 0 project failures.
