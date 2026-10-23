# Compatibility reports

These advisory checks report potential breakages for precompiled C++ and Java
clients, including vendordeps. They do not block PRs or post PR comments.
Both workflows run only in `wpilibsuite/allwpilib`; fork repositories skip them.
PRs from forks targeting the upstream repository still receive the PR check.

- **PR compatibility** compares the proposed merge commit with the target
  branch HEAD recorded in the PR event. It does not compare with a release tag
  or the PR's merge base. A branch that is behind its target branch therefore
  does not appear to remove APIs already added on the target branch.
- **Release compatibility** runs on pushes to every upstream branch, including
  stable branches, merge, squash, and rebase merges, and direct pushes. It
  compares the new HEAD with the nearest `v[0-9]*` tag on that branch's
  first-parent history, strictly before HEAD. Tags on side branches are ignored.
  It does not run on PRs or tag pushes. Missing tags produce an incomplete
  report, never an empty success.

Both call `compatibility.yml` and build both revisions in the same Linux
container with the same Gradle init script. The script selects existing
published Java JAR tasks and top-level C++ release archives, without building
tests, examples, or documentation. Gradle's build cache can reuse unchanged
outputs. It does not alter the normal Gradle, CMake, or Bazel build definitions.
Libraries whose binary and debug information have identical SHA-256 hashes skip
the expensive ABI analysis, after checking that debug information is present.

## Coverage

[libabigail](https://sourceware.org/libabigail/manual/abidiff.html) compares ELF
symbols and DWARF types reachable from exported interfaces, including private
members which affect public class layout. This catches changes with unchanged
symbol names, such as callback storage growing or a member moving within a
same-sized object. Published release archives carry embedded DWARF or adjacent
`.debug` files. Missing debug information is an analysis error, not a symbol-only pass.
Removed libraries are reported too. C++ coverage is Linux x86-64 only; simulation
plugins, tools, thirdparty packages, and the separate ntcore FFI build are excluded.

[japicmp](https://github.com/siom79/japicmp) compares public and protected Java
classes and members, reporting both binary and source incompatibilities. Each
revision supplies its own compile dependency classpath; unresolved types fail
the analysis rather than being ignored. Comparing all published JARs together
also supports APIs moving between modules. The detailed report includes
modifications such as changed generic return types that can affect previously
compiled callers despite unchanged JVM descriptors.

Reports cover the public library surface, rather than filtering to references
in today's vendor binaries. A finding means a client *could* break; it does not
establish that a particular vendordep uses the API. The manual alpha-7 review's
vendor download and usage tracing is not automated here. Header-only C++ types
absent from library DWARF, inline implementation changes, other platform ABIs,
reflection, JNI behavior, and other semantic changes still need review.

## Reading results

The Actions job summary lists libraries and Java classes with findings, with
expandable excerpts of the native diagnostics. Download
the `compatibility-<candidate SHA>` artifact for full C++ text diagnostics, Java
HTML/XML/text, and `results.json`. `changed` means libabigail found an ABI change;
review its diagnostic before treating it as a proven vendor failure.

Findings return success. Tool, dependency, snapshot, or build failures produce
an **INCOMPLETE** report and failing step diagnostics. The job uses
`continue-on-error` to remain advisory even when analysis cannot finish.
No credentials beyond read access to the repository are used, including for
fork PRs. Both checkouts disable credential persistence.

## Local use

Install `abigail-tools`, a JDK matching the build, and the pinned japicmp JAR
from the workflow (verify its SHA-256). Use two separate clean checkouts. From
each checkout, run the same current init script with a distinct output folder:

```sh
./gradlew --init-script /path/to/current/.github/scripts/compatibility.init.gradle \
  compatibilitySnapshot --build-cache -PskipJavaFormat -Ponlylinuxx86-64 \
  -PcompatibilityOutput=/tmp/compatibility/baseline
```

Repeat for the candidate using `/tmp/compatibility/candidate`, then run:

```sh
python3 .github/scripts/compatibility.py compare \
  --base /tmp/compatibility/baseline --head /tmp/compatibility/candidate \
  --base-revision BASE_SHA --head-revision HEAD_SHA \
  --japicmp /path/to/japicmp.jar --output /tmp/compatibility/report
```

Use a fresh report directory for each comparison. To test the comparison engine
with small, compiled regression fixtures (no full WPILib build):

```sh
JAPICMP_JAR=/path/to/japicmp.jar \
  python3 -m unittest discover -s .github/scripts/compatibility_tests -v
```

Without the tools, the integration fixtures are skipped; the Git baseline and
error-handling tests still run. CI installs the tools and runs all fixtures.
