# Item 3: signed dynamic-struct bitfields

- Baseline: `f592035104e45f2bdf413577779f6a7898e4a3b2`; branch
  `fix/dynamic-struct-signed-bitfields`. Confirmed native accessor defect.
- Root cause: `GetFieldImpl()` already shifts and masks the wire value, but
  `DynamicStruct::GetIntField()` interprets its sign using storage size rather
  than declared width. Literal `int16 value:3` bytes `04 00` and `07 00` read
  as 4 and 7 instead of -4 and -1.
- Fix: extend the declared sign bit using the existing mask, then bit-cast to
  `int64_t`. All arithmetic is unsigned; shifts range from 0 through 63.
  Width 64 needs no special shift or signed overflow. Mutable views inherit
  the same accessor.

## Verification

Platform actually exercised: Linux x86_64, GCC 15.3.1, Bazel C++23 native
configuration. No 32-bit, Windows, or macOS run is claimed.
All Bazel commands used `--jobs=2`, with output redirected to `/tmp/issue3-*.log`.
`../../bazelisk` below is the executable in the main checkout.

1. Baseline command:
   `../../bazelisk test //wpiutil/src/test/native:wpiutil_tests --jobs=2 --test_arg='[struct]'`.
   Initial build timed out and was resumed; linking then failed because the
   installed GCC libatomic linker script referenced missing
   `/usr/lib64/libatomic.so.1.2.0`.
   Alternative: copy generated `wpiutil_tests-0.params` to
   `/tmp/issue3-link.params`, removing only `-latomic`, then
   `gcc @/tmp/issue3-link.params` and
   `bazel-bin/wpiutil/src/test/native/wpiutil_tests '[struct]'`.
   Baseline passed: 60 cases, 771 assertions.
2. Red, before implementation:
   `../../bazelisk build //wpiutil/src/test/native:wpiutil_tests --jobs=2`,
   followed by the same relink and
   `bazel-bin/wpiutil/src/test/native/wpiutil_tests '*BitfieldLiteralBytes'`.
   Signed regression failed 22 assertions; unsigned regression passed
   (152 assertions total). The system library had meanwhile become available,
   and this Bazel build also linked successfully without a workaround.
3. Green:
   `../../bazelisk test //wpiutil/src/test/native:wpiutil_tests --jobs=2`.
   Normal Bazel build/test passed: 1000 cases, 23387 assertions.
   Direct `[struct]` run passed: 62 cases, 923 assertions.
4. `git diff --check` passed.

Regression coverage uses literal bytes, not writer/reader round trips:
required 3-bit values; signed width 1; 8/16/32/64-bit storage; shifted fields
with neighboring bits both clear and set; width 63 minimum/-1/maximum and
shifted minimum; width 64 minimum/-1/maximum; full-width signed integers;
unsigned widths 1/3/63/64. Both immutable and mutable signed accessors are
checked. The whole native wpiutil suite, not the entire multi-module repository,
was exercised.

## Compatibility / downstream

Wire format, layout, public signatures, full-width signed reads, and unsigned
reads remain unchanged. Negative narrow signed fields intentionally now return
negative values instead of erroneous positive values. No other numbered item
is changed. Consumers rebuilding against the commit containing this report
can remove the downstream sign-extension workaround; no prerequisite fix from
another item is required. Because the accessor is inline, consumers must
recompile against the updated header. No downstream checkout was needed.
