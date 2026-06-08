// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * The core WPILib module. This defines APIs for interacting with robot sensors and actuators,
 * dashboard applications, and high-level program control frameworks.
 */
open module wpilib.core {
  requires transitive wpilib.math;
  requires transitive wpilib.units;
  requires transitive wpilib.util;
  requires wpilib.hal;
  requires wpilib.ntcore;
  requires wpilib.cscore;
  requires wpilib.cameraserver;

  exports org.wpilib.event;
  exports org.wpilib.sysid;
  exports org.wpilib.framework;
  exports org.wpilib.system;
  exports org.wpilib.opmode;
  exports org.wpilib.preferences;
  exports org.wpilib.counter;
  exports org.wpilib.simulation;
  exports org.wpilib.hardware.expansionhub;
  exports org.wpilib.hardware.discrete;
  exports org.wpilib.hardware.bus;
  exports org.wpilib.hardware.range;
  exports org.wpilib.hardware.pneumatic;
  exports org.wpilib.hardware.motor;
  exports org.wpilib.hardware.rotation;
  exports org.wpilib.hardware.accelerometer;
  exports org.wpilib.hardware.led;
  exports org.wpilib.hardware.power;
  exports org.wpilib.hardware.imu;
  exports org.wpilib.smartdashboard;
  exports org.wpilib.internal;
  exports org.wpilib.driverstation;
  // "Internal" packages shouldn't be exported, but our templates use it :/
  exports org.wpilib.driverstation.internal;
  exports org.wpilib.drive;
}
