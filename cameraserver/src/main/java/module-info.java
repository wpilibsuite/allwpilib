// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Defines APIs for streaming cameras from the robot for use by dashboards and coprocessors and for
 * running OpenCV pipelines. Note that vision processing on the SystemCore may cause decreased
 * performance of the main robot program.
 */
open module wpilib.cameraserver {
  requires transitive wpilib.cscore;
  requires transitive wpilib.ntcore;
  requires org.opencv;
  requires wpilib.core;
  requires wpilib.hal;
  requires wpilib.util;

  exports org.wpilib.vision.process;
  exports org.wpilib.vision.stream;
}
