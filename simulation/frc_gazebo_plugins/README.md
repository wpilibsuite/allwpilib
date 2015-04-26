Notes for building the gazebo plugins
=====================================

the resulting plugins (shared libraries, `.so/.dll`) are delivered to students via the eclipse simulation plugins, and unzipped to `${HOME}/wpilib/simulation/plugins`

## Building
these are built with cmake, because they use the gazebo libraries.
gazebo libraries provide cmake-config files, so cmake is easier to use here.
See top level building.md for how to build
