# WPILib Bazel Support

WPILib is normally built with Gradle, but [Bazel](https://www.bazel.build/) can also be used to increase development speed due to the superior caching ability and the ability to use remote caching and remote execution (on select platforms)


## Prerequisites
- Install [Bazelisk](https://github.com/bazelbuild/bazelisk/releases) and add it to your path. Bazelisk is a wrapper that will download the correct version of bazel specified in the repository. Note: You can alias/rename the binary to `bazel` if you want to keep the familiar `bazel build` vs `bazelisk build` syntax.

## Building
To build the entire repository, simply run `bazel build //...`. To run all of the unit tests, run `bazel test //...`
Other examples:
- `bazel build //wpimath/...` - Builds every target in the wpimath folder
- `bazel test //wpiutil:wpiutil-cpp-test` - Runs only the cpp test target in the wpiutil folder
- `bazel coverage //wpiutil/...` - (*Nix only) - Runs a code coverage report for both C++ and Java on all the targets under wpiutil

## User settings
When invoking bazel, it will check if `user.bazelrc` exists for additional, user specified flags. You can use these settings to do things like always ignore buildin a specific folder, or limiting the CPU/RAM usage during a build.
Examples:
- `build --build_tag_filters=-wpi-example` - Do not build any targets tagged with `wpi-example` (Currently all of the targets in wpilibcExamples and wpilibjExamples contain this tag)
- `build -c opt` - Always build optimized targets. The default compiler flags were chosen to build as fast as possible, and thus don't contain many optimizations
- `build -k` - `-k` is analogous to the MAKE flag `--keep-going`, so the build will not stop on the first error.
- ```
  build --local_ram_resources=HOST_RAM*.5 # Don't use more than half my RAM when building
  build --local_cpu_resources=HOST_CPUS-1 # Leave one core alone
  ```
