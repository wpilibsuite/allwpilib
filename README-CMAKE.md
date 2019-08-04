# WPILib CMake Support

WPILib is normally built with Gradle, however for some systems, such a linux based coprocessors, Gradle doesn't work correctly, especially if cscore is needed, which requires OpenCV. We provide the CMake build for these cases. Although it is supported on Windows, these docs will only go over linux builds.

## Libraries that get built
* wpiutil
* ntcore
* cscore
* cameraserver
* hal
* wpilib

By default, all libraries except for the HAL and WPILib get built with a default cmake setup. The libraries are built as shared libraries, and include the JNI libraries as well as building the Java jars.

## Prerequisites

The most common prerequisite is going to be OpenCV. OpenCV needs to be findable by cmake. On systems like the Jetson, this is installed by default. Otherwise, you will need to build cmake from source and install it.

In addition, if you want JNI and Java, you will need a JDK of at least version 8 installed. In addition, you need a `JAVA_HOME` environment variable set properly and set to the jdk directory.

## Build Options

The following build options are available:

* WITHOUT_JAVA (OFF Default)
  * Enabling this option will disable Java and JNI builds. If this is off, `BUILD_SHARED_LIBS` must be on. Otherwise cmake will error.
* BUILD_SHARED_LIBS (ON Default)
  * Disabling this option will cause cmake to build static libraries instead of shared libraries. If this is off, `WITHOUT_JAVA` must be on. Otherwise cmake will error.
* WITHOUT_CSCORE (OFF Default)
  * Enabling this option will cause cscore to not be built. This will also implicitly disable cameraserver, the hal and wpilib as well, irrespective of their specific options. If this is on, the opencv build requirement is removed.
* WITHOUT_ALLWPILIB (ON Default)
  * Disabling this option will build the hal and wpilib during the build. The HAL is the simulator hal, unless the external hal options are used. The cmake build has no capability to build for the RoboRIO.
* USE_EXTERNAL_HAL (OFF Default)
  * TODO
* EXTERNAL_HAL_FILE
  * TODO

## Build Setup

The WPILib CMake build does not allow in source builds. Because the `build` directory is used by gradle, we recommend a `buildcmake` directory in the root. This folder is included in the gitignore.

Once you have a build folder, run cmake configuration in that build directory with the following command.

```
cmake path/to/allwpilib/root
```

If you want to change any of the options, add `-DOPTIONHERE=VALUE` to the cmake command. This will check for any dependencies. If everything works properly this will succeed. If not, please check out the troubleshooting section for help.

If you want, you can also use `ccmake` in order to visually set these properties as well.
https://cmake.org/cmake/help/v3.0/manual/ccmake.1.html
Here is the link to the documentation for that program.

## Building

Once you have cmake setup. run `make` from the directory you configured cmake in. This will build all libraries possible. If you have a multicore system, we recommend running make with multiple jobs. The usual rule of thumb is 1.5x the number of cores you have. To run a multiple job build, run the following command with x being the number of jobs you want.

```
make -jx
```

## Installing

After build, the easiest way to use the libraries is to install them. Run the following command to install the libraries. This will install them so that they can be used from external cmake projects.

```
sudo make install
```

## Using the installed libraries for C++.

Using the libraries from C++ is the easiest way to use the built libraries.

To do so, create a new folder to contain your project. Add the following code below to a `CMakeLists.txt` file in that directory.

```
cmake_minimum_required(VERSION 3.5)
project(vision_app) # Project Name Here

find_package(wpilib REQUIRED)

add_executable(my_vision_app main.cpp) # exectuable name as first parameter
target_link_libraries(my_vision_app cameraserver ntcore cscore wpiutil)
```

If you are using them, `wpilibc` and `hal` should be added before the `cameraserver` declaration in the `target_link_libraries` function.

Add a `main.cpp` file to contain your code, and create a build folder. Move into the build folder, and run

```
cmake /path/to/folder/containing/CMakeLists
```

After that, run `make`. That will create your executable. Then you should be able to run `./my_vision_app` to run your application.


## Using the installed libraries for Java
TODO

## Troubleshooting
Below are some common issues that are run into when building.

#### Missing OpenCV

If you are missing OpenCV, you will get an error message similar to this.

```
CMake Error at cscore/CMakeLists.txt:3 (find_package):
  By not providing "FindOpenCV.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "OpenCV", but
  CMake did not find one.

  Could not find a package configuration file provided by "OpenCV" with any
  of the following names:

    OpenCVConfig.cmake
    opencv-config.cmake

  Add the installation prefix of "OpenCV" to CMAKE_PREFIX_PATH or set
  "OpenCV_DIR" to a directory containing one of the above files.  If "OpenCV"
  provides a separate development package or SDK, be sure it has been
  installed.
```

If you get that, you need make sure opencv was installed, and then reattempt to configure. If that doesn't work, set the `OpenCV_DIR` variable to the directory where you built OpenCV.

#### Missing Java

If you are missing Java, you will get a message like the following.
```
CMake Error at /usr/share/cmake-3.5/Modules/FindPackageHandleStandardArgs.cmake:148 (message):
  Could NOT find Java (missing: Java_JAVA_EXECUTABLE Java_JAR_EXECUTABLE
  Java_JAVAC_EXECUTABLE Java_JAVAH_EXECUTABLE Java_JAVADOC_EXECUTABLE)
```

If this happens, make sure you have a JDK of at least version 8 installed, and that your JAVA_HOME variable is set properly to point to the JDK.

In addition, if you do not need Java, you can disable it with `-DWITHOUT_JAVA=ON`.
