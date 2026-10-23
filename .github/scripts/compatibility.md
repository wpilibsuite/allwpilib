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

Reports retain the full public library comparison and add vendordep usage evidence.
Header-only C++ types absent from library DWARF, inline implementation changes,
other platform ABIs, reflection, JNI behavior, and other semantic changes still
need review.

## Vendordep downloads and usage

The workflows check out `wpilibsuite/vendor-json-repo` and select the inventory
using the nearest first-parent release tag **at or before the baseline**. For
example, `v2027.0.0-alpha-7` selects `2027_alpha7`, beta/RC tags select `YEARbeta`,
and stable tags select `YEAR`. A missing series is an incomplete check; the tool
does not fall back to another alpha or season. This selection does not change
the PR comparison baseline, which remains the target branch HEAD.

All manifest versions in that series are included. The downloader deduplicates
Maven coordinates, fetches Java JARs and declared Linux x86-64 release C++/JNI
artifacts, and respects static/shared classifiers and JNI `isJar`. It tries the
manifest's Maven repositories in order and uses Artifactory's download endpoint
for FRC Maven. Headers, sources, debug variants, other platforms, and transitive
Maven POM dependencies are not downloaded. Dependencies without a declared Linux
x86-64 binary are listed as out of scope, including header-only dependencies.

The report retains the registry commit, selected manifests, download URLs,
SHA-256 hashes, failed attempts, and unsupported dependencies. Missing or corrupt
artifacts and tracing errors produce **INCOMPLETE** coverage without preventing
the public API comparison. Vendor archives are inspected without executing their
code, and archive paths are not used for extraction. Downloaded binaries are not
uploaded with the report.

- **Java:** constant-pool class/member references and declared descriptors are
  matched to japicmp's incompatible classes and exact old member descriptors.
  Baseline and vendor class hierarchies resolve inherited references. Both
  binary and source-only changes, such as generic return changes, are retained.
  Class-level findings also include subclasses and interface implementers. Every
  multi-release JAR variant is inspected conservatively; this is static evidence,
  not a determination that a particular JVM will load or execute that variant.
- **C++:** shared-library imports and static archive symbols are inspected with
  `nm` and `c++filt`. Imports of symbols removed from the combined WPILib exports
  are exact matches (symbol versions are not distinguished). Demangled symbols
  are also matched to changed interfaces and nonstandard-library type names in
  libabigail diagnostics. Defined inline/template symbols are included because
  they can embed old object layouts. These textual matches are review leads,
  not a type-equivalence or runtime-failure proof; demangling differences and
  stripped/inlined code can hide usage. Static archive references may belong to
  object files that a final robot application never links.

No usage matches means no references were found within this scope. It does not
turn an ABI finding into a compatibility guarantee or establish that a vendor
binary was compatible with the baseline in the first place.

## Reading results

The Actions job summary lists libraries and Java classes with findings, with
expandable excerpts of the native diagnostics. Download
the `compatibility-<candidate SHA>` artifact for full C++ text diagnostics, Java
HTML/XML/text, `vendors-downloads.json`, `vendors-manifests.json`, and
`results.json` (including full vendor reference evidence). `changed` means libabigail found an ABI change;
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
git clone https://github.com/wpilibsuite/vendor-json-repo.git /tmp/vendor-json-repo
python3 .github/scripts/compatibility.py download-vendors \
  --repo /path/to/current --baseline BASE_SHA --inventory /tmp/vendor-json-repo \
  --output /tmp/compatibility/vendors
python3 .github/scripts/compatibility.py compare \
  --base /tmp/compatibility/baseline --head /tmp/compatibility/candidate \
  --base-revision BASE_SHA --head-revision HEAD_SHA \
  --japicmp /path/to/japicmp.jar --vendors /tmp/compatibility/vendors \
  --output /tmp/compatibility/report
```

`download-vendors --series 2027_alpha7` can explicitly override inventory
selection locally. `compare` can omit `--vendors` for a public-surface-only run;
both CI workflows always request vendor tracing. Downloads are refreshed on
each run, and their recorded hashes are verified before tracing.

Use a fresh report directory for each comparison. To test the comparison engine
with small, compiled regression fixtures (no full WPILib build):

```sh
JAPICMP_JAR=/path/to/japicmp.jar \
  python3 -m unittest discover -s .github/scripts/compatibility_tests -v
```

Without the tools, the integration fixtures are skipped; the Git baseline and
error-handling tests still run. CI installs the tools and runs all fixtures.
