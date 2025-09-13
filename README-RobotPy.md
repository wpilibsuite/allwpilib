# robotpy in allwpilb
allwpilib hosts a mirror of RobotPy that can be built with bazel on Linux. The intent of the mirror is to have breaking changes identified early and fixed by the PR creator so that when wpilib releases are made there is much less work required to release a RobotPy version that wraps it. It is not a goal for allwpilib to replace the RobotPy repo; it will still be considered the "source of truth" for python builds and will be responsible for building against all of the applicable architectures and multiple versions of python.

## Build Process
The upstream RobotPy repository uses toml configuration files and semiwrap to produce Meson build scripts. The allwpilib fork uses these toml configuration files to auto generate bazel build scripts. In general, each project (wpiutil, wpimath, etc) defines two pybind extensions; one that simply wraps the native library, and another that adds extension(s) that and contains all of the python files for the library. Both of these subprojects have auto-generated build files; a `robotpy_native_build_info.bzl` for the lidar wraper and `robotpy_pybind_build_info.bzl` which defines the extensions and python library.

## Disabling robotpy builds
Building the robotpy software on top of the standard C++/Java software can result in more than doubling the amount of time it takes to compile. To skip building the robotpy tooling you can add `--config=skip_robotpy` to the command line or to your `user.bazelrc`

# Syncing with robotpy
NOTE: This process is currently unlanded while robotpy gets the 2027 branch stable

[Copybara](https://github.com/google/copybara) is used to maintin synchronization between the upstream robotpy repositories and the allwpilib mirror. Github actions can be manually run which will create pull requests that will update all of the robotpy files between the two repositories. The ideal process is that the allwpilib mirror is always building in CI, and once a release is created the RobotPy team can run the `wpilib -> robotpy` copybara task, make any fine tuned adjustements and create their release. In the event that additional changes are made on the robotpy side, they can run the `robotpy -> wpilib` task to push the updates back to the mirror. However the goal of the mirroring the software here is to be able to more rapidly test changes and will hopefully overwhelmingly eliminate the need for syncs this direction.
