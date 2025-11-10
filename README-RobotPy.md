# robotpy in allwpilb
allwpilib hosts a mirror of RobotPy that can be built with bazel on Linux. The intent of the mirror is to have breaking changes identified early and fixed by the PR creator so that when wpilib releases are made there is much less work required to release a RobotPy version that wraps it. It is not a goal for allwpilib to replace the RobotPy repo; it will still be considered the "source of truth" for python builds and will be responsible for building against all of the applicable architectures and multiple versions of python.

## Build Process
The upstream RobotPy repository uses toml configuration files and semiwrap to produce Meson build scripts. The allwpilib fork uses these toml configuration files to auto generate bazel build scripts. In general, each project (wpiutil, wpimath, etc) defines two pybind extensions; one that simply wraps the native library, and another that adds extension(s) that and contains all of the python files for the library. Both of these subprojects have auto-generated build files; a `robotpy_native_build_info.bzl` for the lidar wraper and `robotpy_pybind_build_info.bzl` which defines the extensions and python library.

## Disabling robotpy builds
Building the robotpy software on top of the standard C++/Java software can result in more than doubling the amount of time it takes to compile. To skip building the robotpy tooling you can add `--config=skip_robotpy` to the command line or to your `user.bazelrc`

# Syncing with robotpy
NOTE: This process is currently unlanded while robotpy gets the 2027 branch stable

[Copybara](https://github.com/google/copybara) is used to maintin synchronization between the upstream robotpy repositories and the allwpilib mirror. Github actions can be manually run which will create pull requests that will update all of the robotpy files between the two repositories. The ideal process is that the allwpilib mirror is always building in CI, and once a release is created the RobotPy team can run the `wpilib -> robotpy` copybara task, make any fine tuned adjustements and create their release. In the event that additional changes are made on the robotpy side, they can run the `robotpy -> wpilib` task to push the updates back to the mirror. However the goal of the mirroring the software here is to be able to more rapidly test changes and will hopefully overwhelmingly eliminate the need for syncs this direction.


# Debugging Build Errors
The build process is highly automated and automatically parses C++ header files to generate pybind11 bindings. Some of these steps here are considered "pregeneration" steps, and the bazel build system will update build files as necessary. If a new header is added, or if the contents of a header file has changed, some of the pregeneration scripts might need to be run. If you encounter an error building `robotpy` code, it is recommended that you go through these steps to make sure everything is set up correctly. The examples are for `wpilibc`, but similar build tasks and tests exist for each wrapped project

## 1. scan-headers
This can be the first source of problems if a new header file has been added. `semiwrap` will look through all of the headers for a library and notify you if a file is not covered by the projects `pyproject.toml` file. Bazel has a test case to ensure the files are up to date.

An example test failure when a new header being introduced. You can run the test with the following command

bazel run //wpilibc:robotpy-wpilib-scan-headers

```
# wpi
ExpansionHub = "wpi/ExpansionHub.hpp"
ExpansionHubMotor = "wpi/ExpansionHubMotor.hpp"
ExpansionHubPidConstants = "wpi/ExpansionHubPidConstants.hpp"
ExpansionHubServo = "wpi/ExpansionHubServo.hpp"

```

To fix this, you can copy the lines from the console, and add them to the pyproject.toml file, located here `wpilibc/src/main/python/pyproject.toml`

## 2. update-yaml
This process parses all of the header files, and creates a representation of the classes / enums / etc in yaml. Occasionally some functions might be ignored or need custom pybind code, which can be added to these files by the user.

There is a bazel task that you can run to automatically update the files:

`bazel run //wpilibc:write_robotpy-wpilib-update-yaml`


## 3. generate-build-info
This step takes the yaml files, and auto generates a bazel build script for the library.

There is a bazel task that you can run to automatically update the files:

`bazel run //wpilibc:robotpy-wpilib-generator.generate_build_info`

## semiwrap errors
If all of these steps above go smoothly and have their tests pass, but the generated cpp files still won't compile, it is possible that either an update needs to be made to the semiwrap tool to handle the new complex functionality, the new functionality can be ignored, or the new functionality might be better handled with a custom pybind11 implementation. In any case, it is best to reach out to the robotpy team for guidance.

## Running multiple projects at once
Each project has its own `scan-headers` and various pregeneration tools, but you can run all of them at once with the following commands. Note: Sometimes if something in the dependency chain for a library fails, these amalgamation commands will also fail. If that happens, fix your way up the dependency chain project by project.

```
# Scan Headers
bazel test //... -k --test_tag_filters=robotpy_scan_headers --build_tests_only

# All pregen
bazel run //:write_robotpy_files

```
