# Item 4: bounded nested struct views

- Base: `f592035104e45f2bdf413577779f6a7898e4a3b2`.
- Branch: `fix/dynamic-struct-nested-views`.
- Status: reproduced native C++ defect, not an API enhancement.

## Cause and fix

Both `DynamicStruct::GetStructField()` and
`MutableDynamicStruct::GetStructField()` used offset-only `subspan()`, returning
all remaining parent bytes. Supply the nested descriptor size as the subspan
length. Offsets, wire format, descriptor identity, and non-owning lifetime and
aliasing semantics are unchanged. Top-level constructor spans are unchanged.

Regression tests cover immutable and mutable array elements with a trailing
field, scalar children with and without a trailing field, extra parent-buffer
bytes, multilevel non-cyclic nesting, and zero-size scalar/array children at
both beginning and end. They check lengths, contents, aliasing pointers, and
whole-span writes leaving neighboring bytes intact. Const access through a
mutable parent is also checked. Existing cycle-rejection tests remain passing.

## Validation (Linux x86_64, native GCC toolchain)

Commands run from this worktree; `BAZEL` below denotes
`/home/virtuald/src/frc/codex/allwpilib/bazelisk`.

1. Baseline: `$BAZEL test //wpiutil/src/test/native:wpiutil_tests --jobs=2 --test_arg='[struct]'`.
   Initially blocked at link: `/usr/sbin/ld: cannot find /usr/lib64/libatomic.so.1.2.0`.
   The installed GCC `libatomic.so` linker script referred to that missing file.
   A first build invocation also hit the command timeout; the resumed build
   completed compilation and exposed the linker error.
2. Baseline/red alternative: copy
   `bazel-out/k8-fastbuild/bin/wpiutil/src/test/native/wpiutil_tests-0.params`
   to `/tmp/issue4-link.params`, removing only its `-latomic` line, then run
   `(cd "$(dirname "$(readlink -f bazel-out)")" && /usr/sbin/gcc @/tmp/issue4-link.params)`.
   Linking succeeded without unresolved symbols. No production code or build
   configuration was changed for this workaround.
   `bazel-bin/wpiutil/src/test/native/wpiutil_tests '[struct]'` passed the
   baseline: **60 cases, 771 assertions**.
3. Added tests before implementation; rebuilt with the same commands.
   `bazel-bin/wpiutil/src/test/native/wpiutil_tests '*Bounded*'` failed:
   **6 cases, 61 failed assertions** (197 total), including oversized spans,
   unexpected contents, and writes changing adjacent bytes.
4. After the two-accessor fix, the system library became available externally
   (this task did not install it). Normal Bazel linking and testing succeeded:
   `$BAZEL test //wpiutil/src/test/native:wpiutil_tests --jobs=2 --nocache_test_results`.
   **PASS**, full native wpiutil suite. Direct execution also passed:
   `bazel-bin/wpiutil/src/test/native/wpiutil_tests '[struct]'`:
   **66 cases, 968 assertions**;
   `bazel-bin/wpiutil/src/test/native/wpiutil_tests`:
   **1004 cases, 23432 assertions**.
5. `git diff --check`: passed.

Build output was redirected to `/tmp/issue4-*-bazel.log`; baseline, red, and
full-suite direct results are in `/tmp/issue4-baseline.log`,
`/tmp/issue4-red.log`, and `/tmp/issue4-green-full.log`. Only Linux x86_64 was
exercised; no 32-bit, other-platform, or repository-wide suite claim is made.

## Compatibility and downstream status

Repository C++ caller search found Glass's `UpdateStructValueSource()` in
`glass/src/libnt/native/cpp/NetworkTables.cpp`: it recursively reads declared
fields and does not depend on oversized spans. `MutableDynamicStruct::SetStructField()`
already copies only the nested descriptor size, so bounded sources remain
compatible. No caller needing migration was found; Glass itself was not built.

Intentional observable change: nested `GetData()` no longer exposes siblings or
parent trailing bytes. External callers deliberately accessing those bytes must
retain/use the parent span instead. The downstream defensive copy-length
adaptation becomes unnecessary with the commit containing this report (both
accessor fixes are required). These inline accessor changes require consumers
to rebuild against the updated header. No other item or downstream-specific
policy is changed.
