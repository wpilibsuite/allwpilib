# ntcore

ntcore is the reimplementation of the NetworkTables protocol for both Java and C++, communicating with the former by way of a JNI interface. ntcore implements v3 of the NetworkTables spec.

## Build Requirements
To build ntcore, a few requirements must be met:

- Platform Native Toolchain - You must have a toolchain for your native platform installed if you wish to build ntcore for your machine. On Windows, this is Visual Studio. On Mac, this is Clang, and on Linux, this is GCC. Your toolchain must support the `-std=c++11` language flag.
- Platform Native JDK - In order to compile ntcore your native platform, you must have the JDK for your platform installed, so that the correct JNI headers can be included.
- ARM Toolchain - To crosscompile ntcore for the roboRIO, you must have the FRC ARM toolchain installed, which can be found [here](http://first.wpi.edu/FRC/roborio/toolchains/).
- CMake (Optional) - If you with to build with CMake, you must install CMake. The build can also be done with Gradle, and the WPILib build server will build with Gradle.

## Building
Gradle is the main build system used by ntcore. All tasks are run with the `gradlew` wrapper, which is included in the root of the repository. All targets that can be accomplished by Gradle are referred to as tasks. The main task available is `build`. To run Gradle, cd into the build directory and run:

```bash
./gradlew build
```

To build just the Native or ARM version, you must access the approriate subproject and build just that version. For example:

```bash
./gradlew :arm:build # Builds just the arm version of ntcore
./gradlew :native:build # Builds just the native version of ntcore
```

The native version of ntcore will run tests on build. The arm version will not, as the current platform likely does not allow running of an ARM binary.

## Testing
By default, tests will be built for the x86 and x64 versions of ntcore, and will be run during any execution of the `build` or `publish` tasks. To skip building and running the tests, use the `-PwithoutTests` command line flag when running Gradle.

## Publishing
To use ntcore in downstream projects as a Maven-style dependency, use the `publish` command. This will publish four artifacts, where platform_name is your current platform (windows, mac, linux):

- edu.wpi.first.wpilib.networktables.cpp:NetworkTables:3.0.0-SNAPSHOT:arm@zip
- edu.wpi.first.wpilib.networktables.cpp:NetworkTables:3.0.0-SNAPSHOT:platform_name@zip
- edu.wpi.first.wpilib.networktables.java:NetworkTables:3.0.0-SNAPSHOT:arm
- edu.wpi.first.wpilib.networktables.java:NetworkTables:3.0.0-SNAPSHOT:platform_name

These are published to ~/releases/maven/development. To publish to a different repo, specify the `-Prepo=repo_name` flag. Valid repos are:

- development - The default repo.
- beta - Publishes to ~/releases/maven/beta.
- stable - Publishes to ~/releases/maven/stable.
- release - Publishes to ~/releases/maven/release.

Most downstream projects that run on the desktop do not depend on the `platform_name` classifier version of ntcore. Rather, they depend on a version with the `desktop` classifier. Normally, this is a version of ntcore built by the FRC Build server that includes binaries for the 3 major operating systems (Windows, Mac, Linux) and 2 major CPU architectures (x86, x86_64). However, if you are locally testing changes to ntcore, you may want to build a version with the `:desktop` classifier for use on your current platform. To do this, specify the `-PmakeDesktop` flag when publishing. For example:

```bash
./gradlew publish -PmakeDesktop
```

When you do a publish of ntcore locally, regardless of whether `-PmakeDesktop` is found, the locally built copy will override all references to networktables dependencies from the FRC Maven server. To undo this, you must delete `~/releases/maven/<repo>/edu/wpi/first/wpilib/networktables`.
