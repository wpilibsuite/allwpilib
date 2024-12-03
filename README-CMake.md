# WPILib CMake Support

WPILib is normally built with Gradle, however for some systems, such as Linux based coprocessors, Gradle doesn't work correctly, especially if cscore is needed, which requires OpenCV. Furthermore, the CMake build can be used for C++ development because it provides better build caching compared to Gradle. We provide the CMake build for these cases. Although macOS is supported, these docs will only go over Linux and Windows builds, but should mostly work for macOS as well. If you are stuck, you can look at the GitHub workflows for any OS to see how it works.

## Libraries that get built
* apriltag
* cameraserver
* cscore
* fieldImages
* hal (simulation HAL only)
* ntcore
* romiVendordep
* simulation extensions
* wpigui
* wpilib (wpilibc, wpilibj, and myRobot)
* wpilibNewCommands
* wpimath
* wpinet
* wpiunits
* wpiutil
* xrpVendordep

## GUI apps that get built
* datalogtool
* glass
* outlineviewer
* roborioteamnumbersetter
* sysid
* halsim_gui (if simulation extensions are enabled)

By default, all libraries get built with a default CMake setup. The libraries are built as shared libraries, and include the JNI libraries as well as building the Java JARs. Data Log Tool and the roboRIO Team Number Setter are only built if libssh is available.

## Prerequisites

The protobuf library and compiler are needed for protobuf generation.

OpenCV needs to be findable by CMake. On systems like the Jetson, this is installed by default. Otherwise, you will need to build OpenCV from source and install it.

If you want JNI and Java, you will need a JDK of at least version 17 installed. In addition, you need a `JAVA_HOME` environment variable set properly and set to the JDK directory.

If you are building with unit tests or simulation modules, you will also need an Internet connection for the initial setup process, as CMake will clone google-test and imgui from GitHub.

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
* `WITH_JAVA` (OFF Default)
  * This option will enable Java and JNI builds. If this is on, `BUILD_SHARED_LIBS` must be on. Otherwise CMake will error.
* `WITH_JAVA_SOURCE` (`WITH_JAVA` Default)
  * This option will build Java source JARs for each enabled Java library. This does not require `WITH_JAVA` to be on, allowing source JARs to be built without the compiled JARs if desired.
* `WITH_NTCORE` (ON Default)
  * This option will cause ntcore to be built. Turning this off will implicitly disable wpinet, and will cause an error if `WITH_WPILIB` is enabled.
* `WITH_SIMULATION_MODULES` (ON Default)
  * This option will build simulation modules.
* `WITH_TESTS` (ON Default)
  * This option will build C++ unit tests. These can be run via `ctest -C <config>`, where `<config>` is the build configuration, e.g. `Debug` or `Release`.
* `WITH_WPILIB` (ON Default)
  * This option will build the HAL and wpilibc/j during the build. The HAL is the simulation HAL, unless the external HAL options are used. The CMake build has no capability to build for the roboRIO.
* `WITH_WPIMATH` (ON Default)
  * This option will build the wpimath library. This option must be on to build wpilib.
* `WITH_PROTOBUF` (ON Default)
  * This option will build with the protobuf library.
* `WITH_WPIUNITS` (`WITH_JAVA` Default)
  * This option will build the wpiunits library. This option must be on to build the Java wpimath library and requires `WITH_JAVA` to also be on.
* `OPENCV_JAVA_INSTALL_DIR`
  * Set this option to the location of the archive of the OpenCV Java bindings (it should be called opencv-xxx.jar, with the x'es being version numbers). NOTE: set it to the LOCATION of the file, not the file itself!
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

Note that if you are cross-compiling, you will need to override the protobuf options manually to point to the libraries for the target platform. Leave the protoc binary location as the path to the binary for the host platform, since protoc needs to execute on the host platform.

## Presets

The WPILib CMake setup has a variety of presets for common configurations and options used. The default sets the generator to Ninja and build directory to `build-cmake`. The other presets are `with-java` (sets `WITH_JAVA=ON`), `sccache` (sets the C/C++ compiler launcher to sccache), and `with-java-sccache` (a comibination of `with-java` and `sccache`.

## Building

Once you have CMake setup. run `cmake --build .` from the directory you configured CMake in. This will build all libraries possible. We recommend running `cmake --build .` with multiple jobs. For allwpilib, a good rule of thumb is one worker for every 2 GB of available RAM. To run a multiple job build, run the following command with x being the number of jobs you want.

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

On Windows, make sure the directories for the libraries you built are on PATH. For wpilib, the default install location is `C:\Program Files (x86)\allwpilib`. If you built other libraries like OpenCV and protobuf from source, install them, and add the install directories to PATH. This ensures CMake can locate the libraries.

You will also want to add the directories where the DLLs are located (usually the `bin` subdirectory of the install directory) to PATH so they can be loaded by your program. If you are using OpenCV and Java, the `opencv_java` DLL is located in either the `lib` subdirectory if you built but didn't install OpenCV, or the `java` subdirectory if you did install OpenCV.

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

## Using the installed libraries for Java

Using the built JARs is more involved than using the C++ libraries, but the additional work involves providing the paths to various libraries and JARs when needed.

Create a new folder to contain your project. Add the following code below to a `CMakeLists.txt` file in that directory.

```cmake
cmake_minimum_required(VERSION 3.11)
project(robot)

find_package(Java REQUIRED COMPONENTS Development)
include(UseJava)
find_package(wpilib REQUIRED)
find_jar(opencvJar opencv-xxx PATHS ENV PATH) # Change to OpenCV version

file(GLOB_RECURSE JAVA_SOURCES *.java)
# If you want Gradle compatibility or you are using one of the templates/examples, comment out the above line and uncomment this line instead:
# file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
add_jar(robot ${JAVA_SOURCES}
    INCLUDE_JARS apriltag_jar cscore_jar hal_jar ntcore_jar wpilibNewCommands_jar wpimath_jar wpinet_jar wpiutil_jar wpiunits_jar wpilibj_jar ${opencvJar})
export_jars(TARGETS robot FILE robot.jar)
```
This includes all the built JARs except for the vendordeps. If you are not using a JAR/library, you may remove it.
Add a `Main.java` file to contain your code, and create a build folder. Move into the build folder, and run

```
cmake /path/to/folder/containing/CMakeLists
```

After that, run `cmake --build .` to create your JAR file. To execute the JAR file, you need to include the wpilib JARs and your JAR in the classpath, and execute your Java program's entry point. If you are using cscore or cameraserver, you also need to include the path to the OpenCV JAR. If you built it from source, it will be in your OpenCV build directory. If it's installed on the system, CMake may find it from PATH, but you will likely need to locate the JAR and manually give CMake the JAR directory. If you are on Linux, you will also need to add the path of the libraries to `LD_LIBRARY_PATH`. This can be done by prepending `LD_LIBRARY_PATH=/path/to/libraries` to the Java command. If you need to add more paths, separate them with colons. The final command should look like `java -cp "robot.jar:/path/to/library_jars/*" main.package.Main`, using a semicolon to separate paths instead of a colon if you are on Windows. If you are on Linux, the final command should look more like `LD_LIBRARY_PATH=/path/to/libraries java -cp robot.jar:/path/to/library_jars/* main.package.Main`.

## Using vendordeps

Vendordeps are not included as part of the `wpilib` CMake package. However, if you want to use a vendordep, you need to use `find_package(VENDORDEP)`, where `VENDORDEP` is the name of the vendordep (case-sensitive), like `xrpVendordep` or `romiVendordep`. Note that wpilibNewCommands, while a vendordep in normal robot projects, is not built as a vendordep in CMake, and is instead included as part of the `wpilib` CMake package. After you used `find_package`, you can reference the vendordep library like normal, either by using `target_link_libraries` for C++ or `add_jar` for Java.

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

#### Missing Java

If you are missing Java, you will get a message like the following.
```
CMake Error at /usr/share/cmake-3.5/Modules/FindPackageHandleStandardArgs.cmake:148 (message):
  Could NOT find Java (missing: Java_JAVA_EXECUTABLE Java_JAR_EXECUTABLE
  Java_JAVAC_EXECUTABLE Java_JAVAH_EXECUTABLE Java_JAVADOC_EXECUTABLE)
```

If this happens, make sure you have a JDK of at least version 8 installed, and that your JAVA_HOME variable is set properly to point to the JDK.

In addition, if you do not need Java, you can disable it with `-DWITH_JAVA=OFF`.

#### Java: Can't find dependent libraries

If one of the libraries can't be found, you will get an error similar to this one:

```
java.io.IOException: wpiHaljni could not be loaded from path or an embedded resource.
        attempted to load for platform /windows/x86-64/
Last Load Error:
C:\Program Files (x86)\allwpilib\bin\wpiHaljni.dll: Can't find dependent libraries
```

If you get this error, that's usually an indication that not all your libraries are in your PATH. The two libraries that should be in your PATH are OpenCV and protobuf. If the error is coming from cscore, it's likely you're missing OpenCV. Otherwise, it's likely you're missing protobuf.

Note that Linux will not have this specific type of error, as it will usually tell you the dependent library you are missing. In that case, you most likely need to add the library to `LD_LIBRARY_PATH`.
