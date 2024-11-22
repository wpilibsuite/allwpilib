// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

module wpilib.apriltag {
  requires transitive wpilib.opencv;
  requires wpilib.math;
  requires wpilib.util;
  requires com.fasterxml.jackson.annotation;
  requires com.fasterxml.jackson.databind;

  exports edu.wpi.first.apriltag;
  exports edu.wpi.first.apriltag.jni;
}
