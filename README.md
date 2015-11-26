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
