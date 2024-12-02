// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The core WPILib module. This defines APIs for interacting with robot sensors and actuators,
 * dashboard applications, and high-level program control frameworks.
 */
module wpilib {
  requires ejml.core;
  requires ejml.simple;
  requires transitive wpilib.math;
  requires transitive wpilib.units;
  requires transitive wpilib.util;
  requires wpilib.hal;
  requires wpilib.ntcore;
  requires wpilib.cscore;
  requires wpilib.cameraserver;

  exports edu.wpi.first.wpilibj;
  exports edu.wpi.first.wpilibj.counter;
  exports edu.wpi.first.wpilibj.drive;
  exports edu.wpi.first.wpilibj.event;
  exports edu.wpi.first.wpilibj.internal;
  exports edu.wpi.first.wpilibj.livewindow;
  exports edu.wpi.first.wpilibj.motorcontrol;
  exports edu.wpi.first.wpilibj.shuffleboard;
  exports edu.wpi.first.wpilibj.simulation;
  exports edu.wpi.first.wpilibj.smartdashboard;
  exports edu.wpi.first.wpilibj.sysid;
  exports edu.wpi.first.wpilibj.util;
}
