// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The WPILib cameraserver module. This defines APIs for streaming cameras from the robot for use by
 * dashboards and coprocessors, and for running OpenCV pipelines (though note that the roboRIO is
 * too slow to run any but the most simple pipelines at a reasonable speed).
 */
open module wpilib.cameraserver {
  requires transitive wpilib.cscore;
  requires transitive wpilib.ntcore;
  requires wpilib.opencv;
  requires wpilib.util;

  exports edu.wpi.first.cameraserver;
  exports edu.wpi.first.vision;
}
