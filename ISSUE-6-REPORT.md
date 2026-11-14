# Item 6: singleton-array declaration identity

## Finding and contract

Target: `f592035104e45f2bdf413577779f6a7898e4a3b2`, branch
`fix/dynamic-struct-array-identity`. Reproduced an API-semantic limitation, not
an established serialization-contract violation: the parser retained only an
extent (default 1), and `IsArray()` meant `GetArraySize() > 1`.

Add `ParsedDeclaration::isArray` and
`StructFieldDescriptor::IsArrayDeclared()` to preserve explicit brackets,
including `[1]`. The parser records this metadata, database construction copies
it, and dependency resolution leaves it intact. Scalars and bitfields are not
array declarations. Extents, offsets, layout calculation and codecs are unchanged.

Keep `IsArray()` semantics unchanged and document the distinction. Repository
C++ caller inspection found six branches in
`glass/src/libnt/native/cpp/NetworkTables.cpp`: boolean, enum, integer, float,
double and nested-struct presentation select scalar versus array behavior using
that query. Silently changing it would change existing UI value shapes. Character
fields use the string codec regardless. No callers are migrated here.
Existing IsArray() calls retain source and behavior compatibility, but this is
not blanket source or binary ABI compatibility: adding a member to the public
ParsedDeclaration aggregate breaks five-element structured bindings. Such callers
must update their bindings; consumers must rebuild against the changed
descriptor/parser headers and library.

## Baseline, red, green

Platform exercised: Linux x86_64, GCC 15.3.1. No 32-bit, Windows or macOS run;
this item introduces no width-dependent conversion. No build blockers occurred.
All commands ran inside the item-6 worktree, with output redirected to `/tmp`.
Let `B=/home/virtuald/src/frc/codex/allwpilib/bazelisk` and
`T=//wpiutil/src/test/native:wpiutil_tests`.

- Baseline: `$B test $T --jobs=2 > /tmp/issue6-baseline-bazel.log 2>&1`:
  full native wpiutil test target passed before changes.
- Runtime red, before implementation:
  `$B test $T --jobs=2 --test_arg='*SingletonDeclarationReproduction*' --test_output=errors > /tmp/issue6-red-reproduction.log 2>&1`:
  one test failed, exactly at `samples[1].IsArray()` (false); scalar and pair
  expectations passed. This demonstrates the limitation, not a request to change
  the legacy query. The final regression uses the additive declaration query
  and separately asserts the legacy behavior.
- API tests written before implementation:
  `$B test $T --jobs=2 > /tmp/issue6-red-api.log 2>&1` failed compilation because
  the new declaration metadata/query did not yet exist.
- Focused green:
  `$B test $T --jobs=2 --test_arg='[struct]' --test_output=errors > /tmp/issue6-green-struct.log 2>&1`:
  **63 cases, 933 assertions passed**.
- Final full native wpiutil suite:
  `$B test $T --jobs=2 --test_output=errors > /tmp/issue6-final.log 2>&1`:
  **1001 cases, 23397 assertions passed**. `git diff --check` passed.

Regression coverage includes scalar/singleton/multi-element numeric, enum,
character and nested declarations; nested definitions both before and after the
parent; parser bitfields; legacy classification, element sizes, offsets, total
sizes, literal-byte decoding and writing the expected wire bytes. No whole-repo
or Glass build was performed; items 1–5 remain outside this change.

## Downstream adaptation

The commit containing this report is the only upstream change required for item
6. After adopting it and exposing `IsArrayDeclared()`, consumers can stop retaining
separate source-language array-identity annotations. Use the declaration query
for tuple/container shape and `GetArraySize()` plus indexed native accessors for
codec operations, including index 0 for singleton arrays. Existing users of
`IsArray()` retain the old scalar-shaped behavior until they opt in. Python tuple
conversion remains binding policy, not native serialization functionality.
