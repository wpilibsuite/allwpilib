// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The WPILib networking module. This defines APIs for sending and receiving data over the robot
 * network for communication with coprocessors and the driver station.
 */
open module wpilib.ntcore {
  requires wpilib.util;
  requires transitive wpilib.datalog; // Part of the public API

  exports edu.wpi.first.networktables;
}
