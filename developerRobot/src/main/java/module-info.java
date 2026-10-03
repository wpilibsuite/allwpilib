// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Defines the developer robot application module used for local WPILib testing and debugging.
 *
 * <p>The module is open so WPILib can reflectively access robot class constructors.
 */
open module wpilib.developerrobot {
  requires wpilib;
  requires wpilib.command2;
  requires wpilib.command3;
  requires wpilib.drivers;
}
