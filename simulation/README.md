## Simulation Directory

Observe the following directory structure

.
|-- frc_gazebo_plugins (contains Gazebo Plugins)
|   |-- clock
|   |-- dc_motor
|   |-- encoder
|   |-- gyro
|   |-- plugins
|   |-- pneumatic_piston
|   |-- potentiometer
|   |-- rangefinder
|   |-- servo
|
|-- frcsim (launches gazebo with model/plugin paths on linux)
|-- JavaGazebo (java library used by java simulation. Equivelant of the C++ gazebo_transport)
|-- SimDS (linux driverstation)

The gazbeo plugins are currently built with CMake.
Eventually they will be built with gradle.

All of this is delivered to students via a zip file

## Building
see the top level building.md
