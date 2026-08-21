// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/** WPILib third-party device drivers. */
module wpilib.drivers {
  requires wpilib.core;
  requires wpilib.hal;
  requires wpilib.math;
  requires wpilib.units;
  requires wpilib.util;

  exports org.wpilib.drivers.motor;
  exports org.wpilib.drivers.odometry;
}
