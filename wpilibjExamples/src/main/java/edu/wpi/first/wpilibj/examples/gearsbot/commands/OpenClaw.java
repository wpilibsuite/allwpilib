/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.TimedCommand;

import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

/**
 * Opens the claw for one second. Real robots should use sensors, stalling
 * motors is BAD!
 */
public class OpenClaw extends TimedCommand {
  public OpenClaw() {
    super(1);
    requires(Robot.m_claw);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    Robot.m_claw.open();
  }

  // Called once after isFinished returns true
  @Override
  protected void end() {
    Robot.m_claw.stop();
  }
}
