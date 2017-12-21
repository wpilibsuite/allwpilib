# ntcore

[![Travis CI Build Status](https://travis-ci.org/wpilibsuite/ntcore.svg?branch=master)](https://travis-ci.org/wpilibsuite/ntcore)
[![Appveyor Build status](https://ci.appveyor.com/api/projects/status/j6k1apuix04m41ch?svg=true)](https://ci.appveyor.com/project/frcjenkins/ntcore)


ntcore is the reimplementation of the NetworkTables protocol for both Java and C++, communicating with the former by way of a JNI interface. ntcore implements v3 of the NetworkTables spec.

## Build Requirements
To build ntcore, a few requirements must be met:

- Platform Native Toolchain - You must have a toolchain for your native platform installed if you wish to build ntcore for your machine. On Windows, this is Visual Studio. On Mac, this is Clang, and on Linux, this is GCC. Your toolchain must support the `-std=c++11` language flag.
- Platform Native JDK - In order to compile ntcore your native platform, you must have the JDK for your platform installed, so that the correct JNI headers can be included.
- ARM Toolchain - To crosscompile ntcore for the roboRIO, you must have the FRC ARM toolchain installed, which can be found [here](http://first.wpi.edu/FRC/roborio/toolchains/).
- Cross Toolchains (coming soon)

## Building
Gradle is the main build system used by ntcore. All tasks are run with the `gradlew` wrapper, which is included in the root of the repository. All targets that can be accomplished by Gradle are referred to as tasks. The main task available is `build`. To run Gradle, cd into the build directory and run:

```bash
./gradlew build
```

This will build the roboRIO ntcore library, in addition to the library for your native platform. Note if the roboRIO compiler cannot be found, the build will skip the roboRIO build. To build for either only the roboRIO, or every platform except the roboRIO, use the following flags:

```bash
-PskipAthena
-PonlyAthena
```

Note if you choose the `onlyAthena` flag, tests will not be ran, as they depend on the current platform being built.

In addition, more platforms can be built. For instance, with additional cross compilers more Arm binaries can be built. In addition, the second bitness for your current platform can be built with an additional flag. To enable every possible platform, use the following flag.

```bash
-PbuildAll
```

If you are building the native version on a 64 bit Linux computer, use a GCC installation which has multilib support enabled (it can compile both 32 and 64 bit programs). The package providing that support on most Linux distributions is called `gcc-multilib`.

By default, debug binaries of the libraries will be built. To switch to instead build release binaries, use the following flag

```bash
-PreleaseBuild
```

### Custom Cross Compilers
Coming soon

## Testing
By default, tests will be built for any native platform, and will be run during any execution of the `build` or `publish` tasks. To skip building and running the tests, use the `-PskipAllTests` command line flag.

## Publishing
to use ntcore in downstream projects as a Maven-style dependency, use the `publish` command. This will publish the following artifact id's:

- edu.wpi.first.ntcore:ntcore-cpp
- edu.wpi.first.ntcore:ntcore-java

The `ntcore-cpp` artifact will contain the following 2 classifiers:

- `headers` (contains C++ headers)
- `sources` (contains C++ sources)

In addition, a classifier will be created for each binary built by the current build. The internal layout of the artifacts will be as follows.

- `/os/arch/shared/` (shared binaries located here)
- `/os/arch/static/` (static binaries located here)

The `ntcore-java` artifact will contain a jar with no classifiers. This is the java jar file. In addition, the following 2 classifiers will be contained

- `sources` (contains Java sources)
- `javadoc` (contains Javadoc sources)

All of these artifacts by default are published to `~/releases/maven/development`. To switch to the release repository (`~/release/maven/release`), use the flag `-PreleaseType=OFFICIAL`.

All downstream projects are configured to use the individual classifier artifacts. The previouse `desktop` classifier does not exist anymore.

When you do a publish of ntcore locally, the locally built copy will override all references to networktables dependencies from the FRC Maven server. To undo this, you must delete `~/releases/maven/<repo>/edu/wpi/first/ntcore`.
