## Components and Location

Simluation is a mix of wpilib and other components.
The wpilib components are in their respective locations in the /wpilibc and /wpilibj directories.

The gazbeo plugins are currently built with CMake.
Eventually they will be built with gradle.

## Simulation delivery and installation

For 2016 FRCSim is only officially supported on Ubuntu 14.04 or greater.
However it is possible on any platform that runs Gazebo.
FRCSim is currently delivered via a zip file published by this project,
and is available at first.wpi.edu/FRC/roborio/maven/PROMOTION_STATUS/edu/wpi/first/wpilib/simulation/simulation/1.0.0/simulation-1.0.0.zip
where PROMOTION_STATUS is one of:
- development (the latest commit merged into wpilib)
- beta (used periodically before release)
- release (used just before kick-off and possibly a few other times)
- stable (used for old releases we want to keep around)

The zip contains most of the contents of ~/wpilib/simulation, such as C++ libraries, jars, and scripts

The exmple robot model files are seperate, and can be found under "file releases" on collabnet.

All together you need eclipse, gazebo, those two zips, g++ 4.9, java8, and a few other small packages.
See the frcsim-installer script or the screensteps on manual install for all the nitty-gritty details

The frcsim-installer script is meant for quick and painless setup on Ubuntu 14.04, 15.04, or 15.10
A manual install is a more tedious process, so this is an easy option for students.

## Building
See the top level README.md.
