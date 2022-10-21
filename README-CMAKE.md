# WPILib CMake Support

WPILib is normally built with Gradle, however for some systems, such as Linux based coprocessors, Gradle doesn't work correctly, especially if cscore is needed, which requires OpenCV. Furthermore, the CMake build can be used for C++ development because it provides better build caching compared to Gradle. We provide the CMake build for these cases. Although it is supported on Windows, these docs will only go over Linux builds.

## Libraries that get built
* wpiutil
* ntcore
* cscore
* cameraserver
* hal
* wpilib
* halsim
* wpigui
* wpimath

By default, all libraries except for the HAL and WPILib get built with a default CMake setup. The libraries are built as shared libraries, and include the JNI libraries as well as building the Java JARs.

## Prerequisites

The most common prerequisite is going to be OpenCV. OpenCV needs to be findable by CMake. On systems like the Jetson, this is installed by default. Otherwise, you will need to build OpenCV from source and install it.

In addition, if you want JNI and Java, you will need a JDK of at least version 11 installed. In addition, you need a `JAVA_HOME` environment variable set properly and set to the JDK directory.

If you are building with unit tests or simulation modules, you will also need an Internet connection for the initial setup process, as CMake will clone google-test and imgui from GitHub.

## Build Options

The following build options are available:

* `WITH_JAVA` (ON Default)
  * This option will enable Java and JNI builds. If this is on, `WITH_SHARED_LIBS` must be on. Otherwise CMake will error.
* `WITH_SHARED_LIBS` (ON Default)
  * This option will cause cmake to build static libraries instead of shared libraries. If this is off, `WITH_JAVA` must be off. Otherwise CMake will error.
* `WITH_CSCORE` (ON Default)
  * This option will cause cscore to be built. Turning this off will implicitly disable cameraserver, the hal and wpilib as well, irrespective of their specific options. If this is off, the OpenCV build requirement is removed.
* `WITH_NTCORE` (ON Default)
  * This option will cause ntcore to be built. Turning this off will implicitly disable wpinet and wpilib as well, irrespective of their specific options.
* `WITH_WPIMATH` (ON Default)
  * This option will build the wpimath library. This option must be on to build wpilib.
* `WITH_WPILIB` (ON Default)
  * This option will build the hal and wpilibc/j during the build. The HAL is the simulator hal, unless the external hal options are used. The cmake build has no capability to build for the RoboRIO.
* `WITH_EXAMPLES` (ON Default)
  * This option will build C++ examples.
* `WITH_TESTS` (ON Default)
  * This option will build C++ unit tests. These can be run via `make test`.
* `WITH_GUI` (ON Default)
  * This option will build GUI items.
* `WITH_SIMULATION_MODULES` (ON Default)
  * This option will build simulation modules, including wpigui and the HALSim plugins.
* `WITH_EXTERNAL_HAL` (OFF Default)
  * TODO
* `EXTERNAL_HAL_FILE`
  * TODO
* `OPENCV_JAVA_INSTALL_DIR`
  * Set this option to the location of the archive of the OpenCV Java bindings (it should be called opencv-xxx.jar, with the x'es being version numbers). NOTE: set it to the LOCATION of the file, not the file itself!

## Build Setup

The WPILib CMake build does not allow in source builds. Because the `build` directory is used by Gradle, we recommend a `build-cmake` directory in the root. This folder is included in the gitignore.

Once you have a build folder, run CMake configuration in that build directory with the following command.

```
cmake path/to/allwpilib/root
```

If you want to change any of the options, add `-DOPTIONHERE=VALUE` to the `cmake` command. This will check for any dependencies. If everything works properly this will succeed. If not, please check out the troubleshooting section for help.

If you want, you can also use `ccmake` in order to visually set these properties as well. [Here](https://cmake.org/cmake/help/v3.0/manual/ccmake.1.html) is the link to the documentation for that program.

## Building

Once you have cmake setup. run `make` from the directory you configured CMake in. This will build all libraries possible. If you have a multicore system, we recommend running `make` with multiple jobs. The usual rule of thumb is 1.5x the number of cores you have. To run a multiple job build, run the following command with x being the number of jobs you want.

```
make -jx
```

The `ninja` generator is also supported, and can be enabled by passing `-GNinja` to the initial `cmake` command.

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

add_executable(my_vision_app main.cpp) # executable name as first parameter
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

In addition, if you do not need Java, you can disable it with `-DWITH_JAVA=OFF`.
