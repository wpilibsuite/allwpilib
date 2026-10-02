// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Defines APIs for connecting to and reading images from cameras connected to the robot and for
 * configuring those cameras.
 */
open module wpilib.cscore {
  requires transitive org.opencv;
  requires transitive wpilib.util;

  exports org.wpilib.vision.camera;
  exports org.wpilib.vision.camera.raw;
}
