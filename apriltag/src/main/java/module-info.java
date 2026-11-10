// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Defines APIs for detecting AprilTag fiducials and calculating pose estimates based on AprilTag
 * locations.
 */
open module wpilib.apriltag {
  requires transitive org.opencv;
  requires transitive wpilib.math;
  requires transitive wpilib.util;

  exports org.wpilib.vision.apriltag;
  exports org.wpilib.vision.apriltag.jni;
}
