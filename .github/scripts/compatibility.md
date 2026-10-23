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

Both call `compatibility.yml`, build each revision using the same Gradle init
script, and compare matching platform/configuration snapshots. Builds use the
same toolchain for each pair: Linux cross containers, Windows 2025, or macOS 15.
The script selects existing published Java JARs, top-level C++/JNI shared
libraries, and HAL simulation plugins, without building tests, examples, or
documentation. Gradle's build cache can reuse unchanged outputs. It does not
alter the normal Gradle, CMake, or Bazel build definitions.

## Coverage

Every target is checked in **release and debug** configurations:

| Build target | Use | Compared architectures |
| --- | --- | --- |
| `linuxsystemcore` | Robot | SystemCore |
| `linuxx86-64` | Simulation | x86-64 |
| `linuxarm64` | Simulation | ARM64 |
| `windowsx86-64` | Simulation | x86-64 |
| `windowsarm64` | Simulation | ARM64 |
| `osxuniversal` | Simulation | x86-64 and ARM64 separately |

Each macOS universal library and debug file is split before comparison. Missing
slices or mismatched snapshot metadata produce incomplete coverage. There are
12 build pairs and 14 comparison jobs; each report identifies its target.

The native inventory includes JNI shared libraries and the separate
`cscore-jnicvstatic` variant. It also includes every published HAL simulation
plugin, including `halsim_gui` and its statically linked glass/wpigui interfaces
used by vendor GUI extensions. Tools, standalone thirdparty packages, and the
separate ntcore FFI build remain excluded.

[libabigail](https://sourceware.org/libabigail/manual/abidiff.html) compares Linux
ELF symbols and DWARF types reachable from exported interfaces, including private
members which affect public class layout. This catches changes with unchanged
symbol names, such as callback storage growing or a member moving within a
same-sized object. Identical binary/debug pairs skip expensive analysis only
after debug information has been checked.

Windows uses LLVM's PE exports and PDB CodeView records, with type indices
resolved to stable names and definitions. macOS uses Mach-O symbols, LLDB DWARF
record layouts and function/data types, plus DWARF virtual method slots. The
snapshot creates flat macOS debug files while the original build objects still
exist. These checks detect removed exports, same-size field rearrangements,
base/virtual layout changes, and typed C-linkage changes with unchanged names.
They conservatively compare named debug records beyond exported interfaces;
internal-only changes may therefore appear. They are not equivalent to
libabigail's reachability analysis. Missing debug information is an analysis
error, never a symbol-only pass. Removed libraries are also reported.

[japicmp](https://github.com/siom79/japicmp) compares public and protected Java
classes and members, reporting both binary and source incompatibilities. Each
revision supplies its own compile dependency classpath; unresolved types fail
the analysis rather than being ignored. Comparing all published JARs together
also supports APIs moving between modules. The detailed report includes
modifications such as changed generic return types that can affect previously
compiled callers despite unchanged JVM descriptors.

Reports retain the full public library comparison and add vendordep usage evidence.
Header-only C++ types absent from debug information, compiler-pruned types,
inline implementation changes, reflection, runtime behavior, and unsupported
platforms still need review. Standard-library and compiler-internal debug
records are not independently compared by the Windows/macOS type checker.

## JNI contracts

The report checks Java native declarations against short and overloaded JNI
export names (including UTF-16 escaping), flags changed native descriptors or
static/instance status, and reports removed `Java_*`, `JNI_OnLoad`, and
`JNI_OnUnload` exports per library. It also reports new unbound declarations in
classes that previously used exported JNI bindings. Native library ABI checks
include JNI implementations, independently of these name checks.

JNI can access private Java definitions, so a separate class-file comparison
includes private classes, methods, constructors, and fields. Baseline and
candidate WPILib binaries, plus all downloaded vendor native libraries and
static archives, are scanned for affected class/member/descriptor string
literals. This can flag stale `FindClass`, `GetMethodID`, `GetFieldID`, and
registration references after a Java definition change. Co-occurrence is
conservative evidence: it does not prove which lookup consumes each literal.

Reports show the number of native declarations with exported bindings. Dynamic
registration, computed/encoded names, non-ASCII string literals, and JNI runtime
semantics are not fully resolved. No native code or JVM-loaded vendor code is
executed. A clean static report is not a runtime JNI compatibility guarantee.

## Vendordep downloads and usage

The workflows check out `wpilibsuite/vendor-json-repo` and select the inventory
using the nearest first-parent release tag **at or before the baseline**. For
example, `v2027.0.0-alpha-7` selects `2027_alpha7`, beta/RC tags select `YEARbeta`,
and stable tags select `YEAR`. A missing series is an incomplete check; the tool
does not fall back to another alpha or season. This selection does not change
the PR comparison baseline, which remains the target branch HEAD.

All manifest versions in that series are included. The downloader deduplicates
Maven coordinates and fetches Java JARs and C++/JNI artifacts for each target
and configuration, respecting static/shared classifiers and JNI `isJar`.
Both hardware and software simulation JNI dependencies are included, so vendor
simulation plugins are traced too. It tries the manifest's Maven repositories
in order and uses Artifactory's download endpoint for FRC Maven. Headers,
sources, and transitive Maven POM dependencies are not downloaded. Dependencies
without a binary declared for the current target are listed as out of scope,
including header-only dependencies.

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
  GNU/LLVM symbol readers and Itanium/MSVC demanglers. Imports of symbols removed
  from the combined WPILib exports are exact matches (symbol versions are not distinguished). Demangled symbols
  are also matched to changed interfaces and nonstandard-library type names in
  native ABI diagnostics. Defined inline/template symbols are included because
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
the `compatibility-<platform>-<architecture>-<configuration>-<candidate SHA>`
artifact for full C++ text diagnostics, Java HTML/XML/text, `vendors-downloads.json`, `vendors-manifests.json`, and
`results.json` (including full vendor reference evidence and JNI findings).
Windows/macOS reports also contain the normalized before/after type snapshots.
`changed` means the native comparison found differences; review its diagnostic
before treating it as a proven vendor failure.

Findings return success. Tool, dependency, snapshot, or build failures produce
an **INCOMPLETE** report and failing step diagnostics. The job uses
`continue-on-error` to remain advisory even when analysis cannot finish.
No credentials beyond read access to the repository are used, including for
fork PRs. Both checkouts disable credential persistence.

## Local use

Install `abigail-tools`, LLVM tools, a JDK matching the build, and the pinned
japicmp JAR from the workflow (verify its SHA-256). Use two separate clean checkouts. From
each checkout, run the same current init script with a distinct output folder:

```sh
./gradlew --init-script /path/to/current/.github/scripts/compatibility.init.gradle \
  compatibilitySnapshot --build-cache -PskipJavaFormat -Ponlylinuxx86-64 \
  -PcompatibilityPlatform=linuxx86-64 -PcompatibilityConfiguration=release \
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

Change `-Ponly...` and `-PcompatibilityPlatform` together for other targets;
Windows ARM64 also needs `-Pbuildwinarm64`. Use
`-PcompatibilityConfiguration=debug` for debug builds, and pass matching
`--platform` and `--configuration` to `download-vendors`. On macOS, run
`compatibility.py prepare-macos --snapshot SNAPSHOT` immediately after each
build. Run `compare` twice with `--architecture x86_64` and `--architecture arm64`.

For desktop comparisons on Linux, CI installs `llvm-19`, `clang-19`, `lld-19`,
`lldb-19`, `python3-lldb-19`, and `liblldb-19-dev`; set `PYTHONPATH=$(lldb-19 -P)`.
The development package supplies the shared-library link required by Debian's
LLDB Python module. LLVM tool paths can be overridden with uppercase names such
as `LLVM_PDBUTIL` or `LLVM_NM`.

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
