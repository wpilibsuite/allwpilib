# WPILib CMake Support

WPILib is normally built with Gradle, however for some systems, such as Linux based coprocessors, Gradle doesn't work correctly, especially if cscore is needed, which requires OpenCV. Furthermore, the CMake build can be used for C++ development because it provides better build caching compared to Gradle. We provide the CMake build for these cases. Although macOS is supported, these docs will only go over Linux and Windows builds, but should mostly work for macOS as well. If you are stuck, you can look at the GitHub workflows for any OS to see how it works. The CMake build does not build Java or Python, only C++.

## Libraries that get built
* apriltag
* cameraserver
* commandsv2
* cscore
* datalog
* fields
* hal (simulation HAL only)
* ntcore
* romiVendordep
* simulation extensions
* wpigui
* wpilib (wpilibc, wpilibj, and developerRobot)
* wpimath
* wpinet
* wpiutil
* xrpVendordep

## GUI apps that get built
* datalogtool
* glass
* outlineviewer
* sysid
* wpical
* halsim_gui (if simulation extensions are enabled)

By default, all libraries get built with a default CMake setup. The libraries are built as shared libraries. Data Log Tool is only built if libssh is available.

## Prerequisites

OpenCV needs to be findable by CMake. On systems like the Jetson, this is installed by default. Otherwise, you will need to build OpenCV from source and install it.

## Build Options

The following build options are available:

* `BUILD_SHARED_LIBS` (ON Default)
  * This option will cause CMake to build static libraries instead of shared libraries. If this is off, `WITH_JAVA` must be off. Otherwise CMake will error.
* `WITH_CSCORE` (ON Default)
  * This option will cause cscore to be built. Turning this off will implicitly disable cameraserver. If this is off, the OpenCV build requirement is removed.
* `WITH_EXAMPLES` (OFF Default)
  * This option will build C++ examples.
* `WITH_GUI` (ON Default)
  * This option will build GUI items. If this is off, and `WITH_SIMULATION_MODULES` is on, the simulation GUI will not be built.
* `WITH_NTCORE` (ON Default)
  * This option will cause ntcore to be built. Turning this off will implicitly disable wpinet, and will cause an error if `WITH_WPILIB` is enabled.
* `WITH_SIMULATION_MODULES` (ON Default)
  * This option will build simulation modules.
* `WITH_TESTS` (ON Default)
  * This option will build C++ unit tests. These can be run via `ctest -C <config>`, where `<config>` is the build configuration, e.g. `Debug` or `Release`.
* `WITH_WPILIB` (ON Default)
  * This option will build the HAL and wpilibc/j during the build. The HAL is the simulation HAL, unless the external HAL options are used. The CMake build has no capability to build for Systemcore.
* `WITH_WPIMATH` (ON Default)
  * This option will build the wpimath library. This option must be on to build wpilib.
* `WITH_WPIUNITS` (`WITH_JAVA` Default)
  * This option will build the wpiunits library. This option must be on to build the Java wpimath library and requires `WITH_JAVA` to also be on.
* `NO_WERROR` (OFF Default)
  * This option will disable the `-Werror` compilation flag for non-MSVC builds.
* `WPILIB_TARGET_WARNINGS`
  * Add compiler flags to this option to customize compiler options like warnings.

## Build Setup

The WPILib CMake build does not allow in source builds. Because the `build` directory is used by Gradle, we recommend a `build-cmake` directory in the root. This folder is included in the gitignore. We support building with Ninja; other options like Makefiles may be broken.

Once you have a build folder, run CMake configuration in the root directory with the following command.

```
cmake --preset default
```

If you want to change any of the options, add `-DOPTIONHERE=VALUE` to the `cmake` command. This will check for any dependencies. If everything works properly this will succeed. If not, please check out the troubleshooting section for help.

If you want, you can also use `ccmake` in order to visually set these properties as well. [Here](https://cmake.org/cmake/help/v3.0/manual/ccmake.1.html) is the link to the documentation for that program. On Windows, you can use `cmake-gui` instead.

## Presets

The WPILib CMake setup has a variety of presets for common configurations and options used. The default sets the generator to Ninja and build directory to `build-cmake`. The other preset is `sccache` (sets the C/C++ compiler launcher to sccache).

## Building

Once you have CMake setup. run `cmake --build .` from the directory you configured CMake in. This will build all libraries possible. We recommend running `cmake --build .` with multiple jobs. For allwpilib, a good rule of thumb is one worker for every 2 GB of available RAM. To run a multi-job build, run the following command with x being the number of jobs you want.

```
cmake --build . --parallel x
```

Note: wpimath takes gigabytes of RAM to compile. Because of this, the compilers may crash while building due to a lack of memory and your computer may slow down. If you have less than 16 GB of RAM available, you may want to consider building it separately first by adding `--target wpimath` and running it with ~3 jobs to prevent crashes from running out of memory.

To build with a certain configuration, like `Debug` or `Release`, add `--config <config>`, where `<config>` is the name of the configuration you want to build with.

## Installing

After build, the easiest way to use the libraries is to install them. Run the following command to install the libraries. This will install them so that they can be used from external CMake projects.

```
sudo cmake --build . --target install
```

## Preparing to use the installed libraries

On Windows, make sure the directories for the libraries you built are on PATH. For wpilib, the default install location is `C:\Program Files (x86)\allwpilib`. If you built other libraries like OpenCV from source, install them, and add the install directories to PATH. This ensures CMake can locate the libraries.

You will also want to add the directories where the DLLs are located (usually the `bin` subdirectory of the install directory) to PATH so they can be loaded by your program.

## Using the installed libraries for C++.

Using the libraries from C++ is the easiest way to use the built libraries.

To do so, create a new folder to contain your project. Add the following code below to a `CMakeLists.txt` file in that directory.

```cmake
cmake_minimum_required(VERSION 3.11)
project(vision_app) # Project Name Here

find_package(wpilib REQUIRED)

add_executable(my_vision_app main.cpp) # executable name as first parameter
target_link_libraries(my_vision_app cameraserver ntcore cscore wpiutil)
```

If you want to use other libraries or are building a robot program, `wpilibc` and `hal` should be added in the `target_link_libraries` function, along with any other libraries you plan on using, e.g. `wpimath`.

Add a `main.cpp` file to contain your code, and create a build folder. Move into the build folder, and run

```
cmake /path/to/folder/containing/CMakeLists
```

After that, run `cmake --build .`. That will create your executable. Then you should be able to run `./my_vision_app` to run your application.

## Using vendordeps

Vendordeps are not included as part of the `wpilib` CMake package. However, if you want to use a vendordep, you need to use `find_package(VENDORDEP)`, where `VENDORDEP` is the name of the vendordep (case-sensitive), like `xrpVendordep` or `romiVendordep`. Note that commandsv2, while a vendordep in normal robot projects, is not built as a vendordep in CMake, and is instead included as part of the `wpilib` CMake package. After you used `find_package`, you can reference the vendordep library like normal (using `target_link_libraries`).

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

If you get that, you need make sure OpenCV was installed, and then reattempt to configure. If that doesn't work, set the `OpenCV_DIR` variable to the directory where you built OpenCV.
