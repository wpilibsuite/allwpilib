// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * Wait until the collector senses that it has the ball. This command does nothing and is intended
 * to be used in command groups to wait for this condition.
 */
public class WaitForBall extends Command {
  public WaitForBall() {
    requires(Robot.collector);
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  protected boolean isFinished() {
    return Robot.collector.hasBall();
  }
}
