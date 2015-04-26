Building everything requires Maven, and the arm jdk

To build eclipse plugins (also build wpilibc/j)
========================
./gradlew eclipsePlugins //this is what is most often used

other possible targets are "build" or "frcuserprogram"


To build on CMake Projects (gz_msgs, frc_gazebo_plugins, WPILIbC++Sim
=========================
    //STARTING FROM ALLWPILIB DIRECTORY
    $> mkdir build
    $> cd build

    // on linux
    $> cmake .. && make -j4

    // on windows
    $> ..\configure.bat
    $> jom

## You can build subprojects of CMake stuff
To find all of them, you can navigate to "build" and type "make <tab> <tab>"
Examples:
  make WPILibC++Sim
  make dc_motor
  make gz_msgs
  make clean

## Note: For the latest version of FRCSim that uses the NI Driverstation, there is a circular dependency between CMake and Gradle projects.
 wpilibc++sim depends on hal which is gradle, but eclipse plugins depends on wpilibc++sim which is cmake
 so if you want everything to work nicely, run:
   ./gradlew :hal:build
   mkdir build; cd build //if you haven't already done so
   cmake .. && make -j4
   ./gradlew eclipsePlugins //or whatever else

Other gradle options
========================
./gradlew tasks //list some available tasks

TODO: Explain maven....
TODO: how to import into eclipse correctly...

GCC versions
------------
Update arm-toolchain.cmake if the triplet changes (eg using Ubuntu repo arm compiler is arm-linux-gnueabi) or in a non-standard location. Currently it assumes that the compiler is on the path.
