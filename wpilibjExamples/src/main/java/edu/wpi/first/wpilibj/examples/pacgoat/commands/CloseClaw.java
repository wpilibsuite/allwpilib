// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.InstantCommand;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * Close the claw.
 *
 * <p>NOTE: It doesn't wait for the claw to close since there is no sensor to
 * detect that.
 */
public class CloseClaw extends InstantCommand {
  public CloseClaw() {
    requires(Robot.collector);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    Robot.collector.close();
  }
}
