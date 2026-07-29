// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Defines command-based framework APIs in the {@code org.wpilib.wpilibj2.command} namespace.
 */
open module wpilib.commands2 {
  requires wpilib.units;
  requires wpilib.core;
  requires wpilib.hal;
  requires wpilib.ntcore;

  exports org.wpilib.wpilibj2.command;
  exports org.wpilib.wpilibj2.command.button;
  exports org.wpilib.wpilibj2.command.sysid;
}
