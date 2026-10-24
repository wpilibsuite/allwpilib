# Compatibility reports

The compatibility workflows compare C++ ABIs, Java definitions, and JNI contracts
between two WPILib revisions. They download vendordeps and trace references to
changed interfaces, covering robot libraries and desktop simulation plugins.
Findings are advisory: they do not block PRs or post PR comments.

## Workflows and baselines

The [Gradle workflow](../workflows/gradle.yml) runs compatibility reporting after
its existing native build jobs. Reporting runs only in `wpilibsuite/allwpilib`;
fork repositories skip it. PRs from forks targeting upstream still receive it.

- **PRs** compare the proposed merge commit with the target branch HEAD recorded
  in the PR event. The baseline is neither a release tag nor the PR's merge base,
  so a branch behind its target does not appear to remove APIs already added there.
- **Branch pushes** compare the new HEAD with the nearest `v[0-9]*` tag on the
  branch's first-parent history, strictly before HEAD. Every upstream branch is
  included, including stable branches, merges, and direct pushes. Tags on side
  branches are ignored. Missing tags produce an incomplete report.
- **Tag pushes and merge-group events** build artifacts but do not run reports.

[compatibility.yml](../workflows/compatibility.yml) selects the baseline and runs
**six report jobs**, one per platform. Each job downloads its candidate Gradle
artifacts and published baseline once and checks both release and debug; macOS
checks both architectures as well. The candidate is never rebuilt for the compatibility check.

The existing `Linux`, `Arm64`, `Systemcore`, `Win64Debug`, `Win64Release`,
`WinArm64Debug`, `WinArm64Release`, and `macOS` artifacts supply the candidate
libraries and JARs. An advisory post-build task adds a `compatibility/` directory
to those same artifacts. It contains the exact commit/platform/configuration
inventory, Java dependency JARs, the omitted Linux `cscore-jnicvstatic` debug
sidecar, and macOS DWARF linked while the build objects still exist. It does not
copy or repackage the published binaries, or compile/link native code. Maven
publishing excludes this directory and the report artifacts.

Baselines come from **Artifactory**, using anonymous
`https://frcmaven.wpi.edu/artifactory/api/download/` requests. Selection searches
`release` and `development`, plus their year-specific counterparts. Maven version
metadata maps development versions' Git hash suffixes and release versions' tags
to commits. The exact requested commit is preferred. If its published version is
missing or incomplete for a platform, selection walks the target branch's
first-parent history from newest to oldest for an available version in the same
year. The candidate itself and commits outside that history are excluded.

Both release and debug packages must be available for every expected publication
on the selected platform. The inventory comes from the selected revision's Gradle
publication declarations, read as text without running them. It covers the same
Java, shared native, JNI and simulation scope as candidate collection, including
separate field-image and cscore JNI packages. Static-only packages and desktop-only
libraries on the robot target are excluded. Artifact bytes are checked against
Artifactory's published SHA-256 checksums. No GitHub baseline artifact, new metadata
file, or successful compatibility-data step is required from historical builds.

When a different baseline is selected, Actions and the report warn with the
requested commit, actual commit, Artifactory repository, and version. Selection
is per platform, so availability can produce different baselines. Comparisons and
vendordep inventory selection use the actual baseline commit. The report retains
publication URLs, checksums, version, commit and reconstructed inventory in
`baseline-provenance.json`. Compiler or toolchain changes between published
builds can also cause ABI differences that require review.

The combined WPILib publication POMs omit Java dependencies. The baseline's version
catalog and compile dependency declarations supply a standalone Maven POM;
Maven resolves its external dependency JARs and transitive dependencies only.
It never invokes a WPILib build. The dependency POM and resolution log are retained
with the report. This reader supports the repository's shared publication scripts
and version-catalog dependency syntax; new publication conventions must update it.

**Compatibility CI never rebuilds either revision.** Missing published binaries,
checksum failures, or missing candidate artifacts fail the platform job with an
**INCOMPLETE** report. Older published packages may lack macOS DWARF or the Linux
`cscore-jnicvstatic` debug sidecar. Those missing debug records are reported for
the affected native libraries; other native, Java and JNI checks still run.
The local snapshot task described below remains available for explicit local use.

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
14 target/configuration comparisons grouped into six jobs; each report identifies
its target.

The native inventory includes JNI shared libraries and the separate
`cscore-jnicvstatic` variant, including its Linux debug sidecar. It also includes
every published HAL simulation plugin, including `halsim_gui` and its statically
linked glass/wpigui interfaces used by vendor GUI extensions. Tools, standalone
thirdparty packages, and the separate ntcore FFI build remain excluded.

[libabigail](https://sourceware.org/libabigail/manual/abidiff.html) compares Linux
ELF symbols and DWARF types reachable from exported interfaces, including private
members which affect public class layout. This catches changes with unchanged
symbol names, such as callback storage growing or a member moving within a
same-sized object. Identical binary/debug pairs skip expensive analysis only
after debug information has been checked.

Windows uses LLVM's PE exports and PDB CodeView records, with type indices
resolved to stable names and definitions. macOS uses Mach-O symbols, LLDB DWARF
record layouts and function/data types, plus DWARF virtual method slots. Collection
uses packaged debug files when present; supplemental macOS debug generation
requires the candidate's original build objects to remain available. These checks
detect removed exports, same-size field rearrangements,
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
using the nearest first-parent release tag **at or before the baseline**. The
tag determines the registry directory:

| Tag format | Registry directory |
| --- | --- |
| `vYEAR.X.Y-alpha-N` | `YEAR_alphaN` |
| `vYEAR.X.Y-beta-N` or `vYEAR.X.Y-rc-N` | `YEARbeta` |
| `vYEAR.X.Y` | `YEAR` |

A missing series is an incomplete check; the tool does not fall back to another
alpha or season. Inventory selection does not change the PR comparison baseline,
which remains the target branch HEAD.

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
the public API comparison. Unavailable downloads are coverage warnings; they do
not fail the comparison step. Invalid inventories, checksum mismatches after
download, and inspection-tool failures remain errors. Vendor archives are
inspected without executing their code, and archive paths are not used for
extraction. Downloaded binaries are not uploaded with the report.

- **Java:** constant-pool class/member references and declared descriptors are
  matched to japicmp's incompatible classes and exact old member descriptors.
  Baseline and vendor class hierarchies resolve inherited references. Both
  binary and source-only changes, such as generic return changes, are retained.
  Class-level findings also include subclasses and interface implementers. Every
  multi-release JAR variant is inspected conservatively; this is static evidence,
  not a determination that a particular JVM will load or execute that variant.
- **C++:** shared-library imports and static archive symbols are inspected with
  GNU/LLVM symbol readers and Itanium/MSVC demanglers. Imports of symbols removed
  from the combined WPILib exports are exact matches (symbol versions are not
  distinguished). Demangled symbols are also matched to changed interfaces and
  nonstandard-library type names in native ABI diagnostics. Defined
  inline/template symbols are included because they can embed old object layouts.
  These textual matches are review leads,
  not a type-equivalence or runtime-failure proof; demangling differences and
  stripped/inlined code can hide usage. Static archive references may belong to
  object files that a final robot application never links. Unsupported MSVC
  mangling retains the raw symbol for exact matching and leaves other symbols
  available for type tracing. These partial demangling results produce coverage
  warnings, with the undecoded symbols recorded in `results.json`.

No usage matches means no references were found within this scope. It does not
turn an ABI finding into a compatibility guarantee or establish that a vendor
binary was compatible with the baseline in the first place.

## Reading results

Each Actions job summary identifies the revisions and target, lists native and
Java findings, shows vendordep usage evidence, and reports JNI findings and
exported-binding counts. Native diagnostics have expandable excerpts. Download
the `compatibility-<platform>-<candidate SHA>`
artifact for full C++ text diagnostics, Java HTML/XML/text,
`vendors-downloads.json`, `vendors-manifests.json`, and `results.json` (including
full vendor reference evidence and JNI findings). Reports are grouped in
`<configuration>/<architecture>/` directories, using `x86_64` and `arm64` for
macOS and `native` for other targets.
Windows/macOS reports also contain the normalized before/after type snapshots.
Packaged debug files take precedence over supplemental copies. When a macOS ZIP
already includes its `.dylib.debug` files, collection uses those files without
running `dsymutil` or requiring the original build objects.

The job log identifies the current stage and native library. Timeouts, startup
failures, and signal termination are retained in the affected text report.

Native library statuses distinguish findings from inspection failures:

| Status | Meaning |
| --- | --- |
| `unchanged` | No changes were detected within the comparison's scope. |
| `added` | A published library exists only in the candidate. |
| `removed` | A published library exists only in the baseline. |
| `changed` | The native comparison found differences requiring review. |
| `error` | The native inspection could not complete. |

Reports consume existing candidate Gradle artifacts and published Artifactory
baselines. There are no compatibility build jobs, build caches, or separately
uploaded build snapshots.

Findings and vendor coverage warnings return success. Coverage warnings remain
marked **INCOMPLETE** in the summary and generate an Actions warning. Tool,
comparison dependency, inventory, checksum, snapshot, or build failures produce
an **INCOMPLETE** report and failing step diagnostics. Machine-readable results
separate `warnings` from `errors`; `requested_base` and `baseline_warning` record
any artifact-baseline deviation. ABI/API findings and analysis diagnostics remain
advisory. Missing or invalid build inputs fail the job; CI does not recover by
compiling a replacement. Setup and regression-test failures also fail the job.
Report jobs use only read access to repository contents and Actions artifacts,
including for fork PRs. Their checkouts disable credential persistence.

## Local use

Build the workflow's pinned libabigail 3.0 source with the
[subrange comparison patch](libabigail-3.0-subrange-comparison.patch) applied,
put its `bin` directory on `PATH`, and make its `lib` directory available through
`LD_LIBRARY_PATH`. Install LLVM tools and a JDK matching the build, and download
the pinned japicmp JAR from the workflow (verify its SHA-256). Use two separate
clean checkouts. From each checkout, run the same current init script with a
distinct output folder:

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
  --platform linuxx86-64 --configuration release \
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
Use a separate report directory for each architecture.

For desktop comparisons on Linux, CI installs `llvm-19`, `clang-19`, `lld-19`,
`lldb-19`, `python3-lldb-19`, and `liblldb-19-dev`; set `PYTHONPATH=$(lldb-19 -P)`.
The development package supplies the shared-library link required by Debian's
LLDB Python module. LLVM tool paths can be overridden with uppercase names such
as `LLVM_PDBUTIL` or `LLVM_NM`.

`download-vendors --series SERIES` can explicitly override inventory selection
locally, where `SERIES` is a registry directory matching the formats above.
`compare` can omit `--vendors` to compare WPILib libraries and JNI contracts
without vendor usage tracing. All CI reports request vendor tracing.
Downloads are refreshed on each run, and their recorded hashes are verified
before tracing.

Use a fresh report directory for each comparison. To test the comparison engine
with small, compiled regression fixtures (no full WPILib build):

```sh
JAPICMP_JAR=/path/to/japicmp.jar \
  python3 -m unittest discover -s .github/scripts/compatibility_tests -v
```

Without the tools, the integration fixtures are skipped; the Git baseline and
error-handling tests still run. CI installs the tools and runs all fixtures.
The compiled fixtures cover native layouts and removed symbols, Windows/macOS
architecture handling and virtual methods, C-linkage function/data types, Java
definition changes, JNI declarations and private-member references, and vendor
usage tracing. Additional tests cover registry selection, downloads, integrity
checks, artifact provenance, recent baseline selection, supplemental debug files,
grouped configuration and architecture coverage, and failures without rebuilds.
