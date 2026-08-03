// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Defines APIs for detecting AprilTag fiducials and calculating pose estimates based on AprilTag
 * locations.
 */
open module wpilib.apriltag {
  requires org.opencv; // not transitive; users can import the opencv module explicitly
  requires wpilib.math;
  requires wpilib.util;

  exports org.wpilib.vision.apriltag;
  exports org.wpilib.vision.apriltag.jni;
}
