// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/** Opens the claw. */
public class OpenClaw extends Command {
  public OpenClaw() {
    requires(Robot.collector);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    Robot.collector.open();
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  protected boolean isFinished() {
    return Robot.collector.isOpen();
  }
}
