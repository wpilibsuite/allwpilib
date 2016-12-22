# WPILib Project

[![Build Status](https://travis-ci.org/wpilibsuite/allwpilib.svg?branch=master)](https://travis-ci.org/wpilibsuite/allwpilib)

Welcome to the WPILib project. This repository contains the HAL, WPILibJ, and WPILibC projects. These are the core libraries for creating robot programs for the roboRIO.

- [WPILib Mission](#wpilib-mission)
- [Building WPILib](#building-wpilib)
    - [Requirements](#requirements)
    - [Setup](#setup)
    - [Building](#building)
    - [Publishing](#publishing)
    - [Structure and Organization](#structure-and-organization)
- [Contributing to WPILib](#contributing-to-wpilib)

## WPILib Mission

The WPILib Mission is to enable FIRST Robotics teams to focus on writing game-specific software rather than focussing on hardware details - "raise the floor, don't lower the ceiling". We work to enable teams with limited programming knowledge and/or mentor experience to be as successful as possible, while not hampering the abilities of teams with more advanced programming capabilities. We support Kit of Parts control system components directly in the library. We also strive to keep parity between major features of each language (Java, C++, and NI's LabVIEW), so that teams aren't at a disadvantage for choosing a specific programming language. WPILib is an open source project, licensed under the BSD 3-clause license. You can find a copy of the license [here](license.txt).

# Building WPILib

Using Gradle makes building WPILib very straightforward. It only has a few dependencies on outside tools, such as the ARM cross compiler for creating roboRIO binaries.

## Requirements

- [ARM Compiler Toolchain](http://first.wpi.edu/FRC/roborio/toolchains/)
- Doxygen (Only required if you want to build the C++ documentation)
- [wpiformat](https://github.com/wpilibsuite/styleguide)

## Setup

Clone the WPILib repository. If the toolchains are not installed, install them, and make sure they are available on the system PATH.

See the [styleguide README](https://github.com/wpilibsuite/styleguide/blob/master/README.md) for wpiformat setup instructions.

## Building

All build steps are executed using the Gradle wrapper, `gradlew`. Each target that Gradle can build is referred to as a task. The most common Gradle task to use is `build`. This will build all the outputs created by WPILib. To run, open a console and cd into the cloned WPILib directory. Then:

```bash
./gradlew build
```

To build a specific subproject, such as WPILibC, you must access the subproject and run the build task only on that project. Accessing a subproject in Gradle is quite easy. Simply use `:subproject_name:task_name` with the Gradle wrapper. For example, building just WPILibC:

```bash
./gradlew :wpilibc:build
```

If you have installed the FRC Toolchain to a directory other than the default, or if the Toolchain location is not on your System PATH, you can pass the `toolChainPath` property to specify where it is located. Example:

```bash
./gradlew build -PtoolChainPath=some/path/to/frc/toolchain/bin
```

If you also want simulation to be built, add -PmakeSim. This requires gazebo_transport. We have tested on 14.04 and 15.05, but any correct install of Gazebo should work, even on Windows if you build Gazebo from source. Correct means CMake needs to be able to find gazebo-config.cmake. See [The Gazebo website](https://gazebosim.org/) for installation instructions.

```bash
./gradlew build -PmakeSim
```

If you prefer to use CMake directly, the you can still do so.
The common CMake tasks are wpilibcSim, frc_gazebo_plugins, and gz_msgs

```bash
mkdir build #run this in the root of allwpilib
cd build
cmake ..
make
```

The gradlew wrapper only exists in the root of the main project, so be sure to run all commands from there. All of the subprojects have build tasks that can be run. Gradle automatically determines and rebuilds dependencies, so if you make a change in the HAL and then run `./gradlew :wpilibc:build`, the HAL will be rebuilt, then WPILibC.

There are a few tasks other than `build` available. To see them, run the meta-task `tasks`. This will print a list of all available tasks, with a description of each task.

wpiformat can be executed anywhere in the repository via `py -3 -m wpiformat` on Windows or `python3 -m wpiformat` on other platforms.

## Publishing

If you are building to test with the Eclipse plugins or just want to export the build as a Maven-style dependency, simply run the `publish` task. This task will publish all available packages to ~/releases/maven/development. If you need to publish the project to a different repo, you can specify it with `-Prepo=repo_name`. Valid options are:

- development - The default repo.
- beta - Publishes to ~/releases/maven/beta.
- stable - Publishes to ~/releases/maven/stable.
- release - Publishes to ~/releases/maven/release.

The following maven targets a published by this task:

- edu.wpi.first.wpilib.cmake:cpp-root:1.0.0 - roboRIO C++
- edu.wpi.first.wpilibc.simulation:WPILibCSim:0.1.0 - Simulation C++
- edu.wpi.first.wpilibj:wpilibJavaFinal:0.1.0-SNAPSHOT - roboRIO Java
- edu.wpi.first.wpilibj:wpilibJavaSim:0.1.0-SNAPSHOT - Simulation Java
- edu.wpi.first.wpilibj.simulation:SimDS:0.1.0-SNAPSHOT - The driverstation for controlling simulation.
- org.gazebosim:JavaGazebo:0.1.0-SNAPSHOT - Gazebo protocol for Java.

## Structure and Organization

The main WPILib code you're probably looking for is in WPILibJ and WPILibC. Those directories are split into shared, sim, and athena. Athena contains the WPILib code meant to run on your roboRIO. Sim is WPILib code meant to run on your computer with Gazebo, and shared is code shared between the two. Shared code must be platform-independent, since it will be compiled with both the ARM cross-compiler and whatever desktop compiler you are using (g++, msvc, etc...).

The Simulation directory contains extra simulation tools and libraries, such as gz_msgs and JavaGazebo. See sub-directories for more information.

The integration test directories for C++ and Java contain test code that runs on our test-system. When you submit code for review, it is tested by those programs. If you add new functionality you should make sure to write tests for it so we don't break it in the future.

The hal directory contains more C++ code meant to run on the roboRIO. HAL is an acronym for "Hardware Abstraction Layer", and it interfaces with the NI Libraries. The NI Libraries contain the low-level code for controlling devices on your robot. The NI Libraries are found in the ni-libraries folder.

The [styleguide repository](https://github.com/wpilibsuite/styleguide) contains our style guides for C++ and Java code. Anything submitted to the WPILib project needs to follow the code style guides outlined in there. For details about the style, please see the contributors document [here](CONTRIBUTING.md#coding-guidelines).

# Contributing to WPILib

See [CONTRIBUTING.md](CONTRIBUTING.md).
