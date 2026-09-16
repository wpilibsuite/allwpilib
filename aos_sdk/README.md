# AOS SDK

AOS for projects that build with CMake instead of Bazel. It is published through
wpilib's maven pipeline like wpilib's other C++ libraries:

| Artifact                 | Classifiers                                                                                                                                | Contents                                                                                                                                                                                         |
| ------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `org.wpilib.aos:aos-sdk` | `headers`                                                                                                                                  | Every header the public AOS libraries need, from AOS and flatbuffers, including generated ones. Also `cmake/` and `share/aos/`, which are the same on every platform.                       |
| `org.wpilib.aos:aos-sdk` | `linuxsystemcorestatic`, `linuxx86-64static`, `osxuniversalstatic`, `windowsx86-64static`, `windowsarm64static`, and their `debug` variants | `<os>/<arch>/static/libaos.a`, and `<os>/<arch>/static/alwayslink/`.                                                                                                                             |
| `org.wpilib.aos:aos-tools` | `linuxx86-64`, `osxuniversalstatic`, `windowsx86-64static`, `windowsarm64static`                                                          | `<os>/<arch>/flatc`, `generate` and `config_flattener`.                                                                                                                                          |

libuv is in neither the archive nor the headers. It comes from wpinet.

## Using it

Unzip the headers, the static zip for the platform you build for, and the tools
for the platform you build on, all into one directory:

```sh
mkdir aos
unzip aos-sdk-<version>-headers.zip -d aos
unzip aos-sdk-<version>-linuxx86-64static.zip -d aos
unzip aos-tools-<version>-linuxx86-64.zip -d aos
cmake -S . -B build -DAos_DIR=$PWD/aos/cmake
```

```cmake
find_package(Aos REQUIRED)

aos_static_flatbuffer(ping_fbs SRCS ping.fbs)

aos_config(pingpong_config SRC pingpong.json FLATBUFFERS ping_fbs)

add_executable(ping ping.cc)
target_link_libraries(ping PRIVATE aos::aos ping_fbs)
```

`find_package(Aos)` defines the `aos::aos` imported target, finds the three code
generators, and pulls in `aos_static_flatbuffer()` and `aos_config()`.

The SDK also carries aosnt's NetworkTables types from `aosnt/types/`, such as
`wpi.aosnt.Boolean`. Include `aosnt/types/boolean_static.h` and so on to send
them. A config can name them without listing a schema.

## Cross-compiling

The archive comes from the platform you build for, and the tools come from the
platform CMake runs on. Cross-compiling means unzipping two platforms side by
side. For a SystemCore, built on x86-64 Linux:

```sh
mkdir aos
unzip aos-sdk-<version>-headers.zip -d aos
unzip aos-sdk-<version>-linuxsystemcorestatic.zip -d aos
unzip aos-tools-<version>-linuxx86-64.zip -d aos
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=<systemcore>/toolchain-config.cmake \
    -DAos_DIR=$PWD/aos/cmake
```

`AosConfig.cmake` finds the archive in `linux/systemcore/static/` from
`CMAKE_SYSTEM_NAME` and `CMAKE_SYSTEM_PROCESSOR`, and the tools in
`linux/x86-64/` from the host. Set `AOS_PLATFORM` or `AOS_HOST_PLATFORM` to
override either one. Use `Aos_DIR` rather than `CMAKE_PREFIX_PATH`, because the
SystemCore toolchain file confines `find_package` to its sysroot.

## Building it locally

```sh
bazel build -c opt //aos_sdk:aos-sdk-hdrs-zip \
    //aos_sdk:aos-sdk_static_zip-opt-arch-linux-x86-64 \
    //aos_sdk:aos-tools_zip-opt-arch-linux-x86-64
```

Bazel prints where it wrote each zip. Unzip them the same way.

A static zip holds the archive for the platform Bazel builds for, whatever the
target's name says. Build another platform's in its own invocation with that
platform's config, `--config=systemcore` or `--config=windows_arm`:

```sh
bazel build -c opt --config=systemcore \
    //aos_sdk:aos-sdk_static_zip-opt-arch-systemcore
```

## Linking whole archives

`static/alwayslink/` holds an archive of the objects from libraries Bazel marks
`alwayslink = 1`, which one archive cannot express. `AosConfig.cmake` links it
whole so a consumer's binaries behave the way Bazel-built AOS binaries do. The
set is walked out of `CcInfo.linking_context`, so a new alwayslink dependency
comes along on its own, and the archive is built by the same rule as
`libaos.a`, so on macOS it is universal the same way.

## Keeping it honest

Nothing in the shipped CMake is hand-copied out of the Bazel rules.
`cmake/AosGeneratedSettings.cmake` is written by `//aos_sdk:cmake_settings` and
carries the three things that must agree:

- `AOS_FLATC_ARGS`, straight from `DEFAULT_FLATC_ARGS` in
  `@aos//aos/flatbuffers:defs.bzl`. Generated code is not compatible across a
  change here.
- `AOS_COMPILE_DEFINITIONS`, read out of the `CcInfo` of `:aos_runtime`. These
  are not cosmetic: alongside `AOS_OS_NONE`, which `aos/macros.h` hard-errors
  without, they include `FLATBUFFERS_MAX_ALIGNMENT` and the lockless-queue
  layout defines. A consumer compiling without them gets headers that silently
  disagree with the archive they link against.
- `AOS_CXX_STANDARD`, read off the toolchain's C++ compile command line, so
  `aos::aos` asks consumers for the standard the archive was compiled with.

What a consumer links besides the archives differs per platform, so it ships in
the static zip instead, as `<os>/<arch>/static/AosLinkOptions.cmake`, written by
`//aos_sdk:link_options`. `AOS_LINK_OPTIONS` is every `linkopts` entry reachable
from `:aos_runtime` in that platform's configuration -- `-lrt` on Linux,
CoreFoundation on macOS, `ws2_32.lib` on Windows -- each wrapped in
`$<LINK_ONLY:>` so it reaches a link but not an archive that combines `aos::aos`
with others.

The header list and include paths come from `CcInfo` as well, so they track
dependency changes on their own. Widening the SDK is a matter of adding to
`:aos_runtime` in `aos_sdk/BUILD.bazel`.
