// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The WPILib cscore module. This defines APIs for connecting to and reading images from cameras
 * connected to the robot and for configuring those cameras.
 */
open module wpilib.cscore {
  requires transitive wpilib.opencv;
  requires wpilib.util;

  exports edu.wpi.first.cscore;
  exports edu.wpi.first.cscore.raw;
}
