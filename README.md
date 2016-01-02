# Building WPILib

Building WPILib is very straightforward. WPILib uses Gradle to compile. It only has a few dependencies on outside tools, including the ARM cross compiler.

## Requirements
- [ARM Compiler Toolchain](http://first.wpi.edu/FRC/roborio/toolchains/)
- Doxygen (Only required if you want to build the C++ documentation)

## Setup
Clone the WPILib Repo. If the toolchains are not installed, install them, and make sure they are available on the system PATH.

## Building
All build steps are executed using the Gradle wrapper, `gradlew`. Each target that Gradle can build is referred to as a task. The most common Gradle task to use is `build`. This will build all of the outputs created by WPILib. To run, open a console and cd into the cloned WPILib directory. Then:

```bash
./gradlew build
```

To build a specific subproject, such as wpilibc, you must access the subproject and run the build task only on that project. Accessing a subproject in Gradle is quite easy. Simply use `:subproject_name:task_name` with the Gradle wrapper. For example, building just wpilibc:

```bash
./gradlew :wpilibc:build
```

If you also want simulation to be build, add -PmakeSim. This requires gazebo_transport. We have tested on 14.04 and 15.05, but any correct install of gazebo should work, even on windows if you build from source. Correct means cmake needs to be able to find gazebo-config.cmake, which you get for free with ubuntu installs.

```bash
./gradlew build -PmakeSim
```

C++ simulation tasks (including plugins, gz_msgs, and wpilibcSim) all depend on gazebo_transport. In order for this to build you must have installed gazebo. See [The Gazebo website](https://gazebosim.org/) for installation instructions.
If you prefer to use CMake directly, the you can still do so.
The common cmake tasks are wpilibcSim, frc_gazebo_plugins, and gz_msgs

```bash
mkdir build #run this in the root of allwpilib
cd build
cmake ..
make
```


The gradlew wrapper only exists in the root of the main project, so be sure to run all commands from there. All of the subprojects have build tasks that can be run. Gradle automatically determines and rebuilds dependencies, so if you make a change to the HAL and then run `:wpilibc:build`, the HAL will be rebuilt, then wpilibc.

There are a few tasks other than `build` available. To see them, run the meta-task `tasks`. This will print a list of all available tasks, with a description of each task.

## Publishing
If you are building to test with the eclipse plugins or just want to export the build as a Maven-style dependency, simply run the `publish` task. This task will publish all available packages to ~/releases/maven/development. If you need to publish the project to a different repo, you can specify it with `-Prepo=repo_name`. Valid options are:

- development - The default repo.
- beta - Publishes to ~/releases/maven/beta.
- stable - Publishes to ~/releases/maven/stable.
- release - Publishes to ~/releases/maven/release.

The following maven targets a published by this task:

- edu.wpi.first.wpilib.cmake:cpp-root:1.0.0 - RoboRIO C++
- edu.wpi.first.wpilibc.simulation:WPILibCSim:0.1.0 - Simulation C++
- edu.wpi.first.wpilibj:wpilibJavaFinal:0.1.0-SNAPSHOT - RoboRIO Java
- edu.wpi.first.wpilibj:wpilibJavaSim:0.1.0-SNAPSHOT - Simulation Java
- edu.wpi.first.wpilibj.simulation:SimDS:0.1.0-SNAPSHOT - The driverstation for controlling simulation.
- org.gazebosim:JavaGazebo:0.1.0-SNAPSHOT - Gazebo protocol for Java.

## Structure and Organization
The main wpilib code you're probably looking for is in wpilibj and wpilibc. Those directories are split into shared, sim, and athena. Athena contains the wpilib code meant to run on your RoboRIO. Sim is wpilib code meant to run on your computer with gazebo, and shared is code shared between the two. Shared code must be platform-independent, since it will be compiled with both the ARM cross-compiler and whatever desktop compiler you are using (g++, msvc, etc...).

The Simulation directory contains extra simulation tools and libraries, such as gz_msgs and JavaGazebo. See sub-directories for more information.

The integration test directories for C++ and Java contain test code that runs on our test-system. When you submit code for review, it is tested by those programs. If you add new functionality you should make sure to write tests for it so we don't break it in the future.

The hal directory contains more C++ code meant to run on the RoboRIO. HAL is an acronym for "Hardware Abstraction Layer", and it interfaces with the NI Libraries. The NI Libraries contain the low-level code for controlling devices on your robot. The NI Libraries are found in the ni-libraries folder.

The styleguide directory contains the styleguide for C++ and Java code. Anything submitted to the wpilib project needs to follow the code style guides outlined in there.