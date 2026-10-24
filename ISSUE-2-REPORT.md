# Item 2: checked dynamic-struct layouts

Base: `f592035104e45f2bdf413577779f6a7898e4a3b2`.
Branch: `fix/dynamic-struct-layout-overflow` (no other issue branch required).

Confirmed defect: unchecked multiplication and offset addition accepted wrapped
layouts, including dependency completion. New tests failed with 13 assertions
before implementation. Layout construction now checks multiplication and every
addition (including bitfield transitions and final storage). On failure affected
descriptors are invalid, never usable wrapped layouts. Zero-size types and very
large schemas remain legal; no allocation/materialization limit was added.

Tests on Linux x86-64, GCC 15:

* Baseline native executable: 998 cases, 23235 assertions passed.
* Regression red: 13 failing assertions in `DynamicStructTest Layout overflow`.
* Fixed native executable: 1000 cases, 23258 assertions passed.
* Tests cover multiplication, cumulative addition using parser-accepted lengths,
  all three bitfield addition sites, unresolved dependency overflow, exact
  SIZE_MAX layouts, zero-size arrays and nesting. Existing nesting, arrays and
  bitfield suites also pass.
* Review exposed an existing undefined shift when constructing nested fields
  larger than eight bytes. The SIZE_MAX nesting test fails with SIGILL under
  `-fsanitize=shift -fsanitize-undefined-trap-on-error` before skipping scalar
  mask computation for struct fields. With the correction, all 1000 cases and
  23258 assertions pass with the production DynamicStruct translation unit
  shift-instrumented. Logs: `/tmp/issue2-ubsan-{red,green}.log`. Ordinary UBSan
  runtime linking is also blocked by a missing host libubsan; trap mode avoids
  that dependency. Nested structs have no scalar bit mask (now consistently 0).
* Real GCC `-m32` executable ran successfully (not a simulated width). Assert-based
  smoke exercised multiplication, addition, bitfield final storage, dependent
  overflow, empty structs, zero-size arrays and exact UINT32_MAX layout.

Commands/logs:

```
../../bazelisk test //wpiutil/src/test/native:all --jobs=2 > /tmp/issue2-green-build.log 2>&1
```

Normal Bazel linking is environmentally blocked: GCC's libatomic linker script
references missing `/usr/lib64/libatomic.so.1.2.0`. No repository build changes
were made. The focused alternative actually runs the **entire** compiled native
Catch2 suite: copy `bazel-out/k8-fastbuild/bin/wpiutil/src/test/native/wpiutil_tests-0.params`
to `/tmp/issue2-link.params`, remove only its `-latomic` line, run
`gcc @/tmp/issue2-link.params` from the Bazel execution root, then run
`bazel-out/k8-fastbuild/bin/wpiutil/src/test/native/wpiutil_tests`.
There are no unresolved atomic symbols. Logs: `/tmp/issue2-baseline.log`,
`/tmp/issue2-red.log`, `/tmp/issue2-green.log`.

32-bit focused source `/tmp/layout32.cpp` uses the public database API and a
`static_assert(sizeof(size_t)==4)`. Build/run:

```
g++ -m32 -std=c++23 -ffunction-sections -fdata-sections \
  -Iwpiutil/src/main/native/include \
  -Iwpiutil/src/main/native/thirdparty/llvm/include /tmp/layout32.cpp \
  wpiutil/src/main/native/cpp/struct/{DynamicStruct,SchemaParser}.cpp \
  wpiutil/src/main/native/thirdparty/llvm/cpp/llvm/{SmallVector,raw_ostream,ConvertUTF,ConvertUTFWrapper,StringExtras,MemAlloc,ErrorHandling}.cpp \
  -Wl,--gc-sections -o /tmp/layout32
/tmp/layout32
```

Compatibility: previously overflowing schemas now return nullptr and an overflow
error. Valid wire layouts and native descriptor addresses are unchanged.
Failure-atomic insertion is deliberately a separate item: this branch alone can
leave rejected definitions registered/invalid. The dependent
`fix/dynamic-struct-insertion-atomicity` branch supplies that contract. Consumers
can drop overflow inspection with this commit, but must retain candidate-database
rebuilding for failed insertion recovery until item 5 is also integrated.
