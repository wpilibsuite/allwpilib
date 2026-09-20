// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/** Defines APIs for event-based programming and concurrent control of mechanisms. */
open module wpilib.command2 {
  requires transitive wpilib.core;
  requires transitive wpilib.units;
  requires wpilib.annotation;
  requires wpilib.util;
  requires wpilib.ntcore;
  requires wpilib.hal;
  requires wpilib.telemetry;
  requires wpilib.tunable;

  exports org.wpilib.command2;
  exports org.wpilib.command2.button;
  exports org.wpilib.command2.sysid;
}
