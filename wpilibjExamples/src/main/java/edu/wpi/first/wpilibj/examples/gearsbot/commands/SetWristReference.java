/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.Command;

import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

/**
 * Move the wrist to a given angle. This command finishes when it is within the
 * tolerance, but leaves the PID loop running to maintain the position. Other
 * commands using the wrist should make sure they disable PID!
 */
public class SetWristReference extends Command {
  private final double m_reference;

  public SetWristReference(double reference) {
    m_reference = reference;
    requires(Robot.m_wrist);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    Robot.m_wrist.enable();
    Robot.m_wrist.setReference(m_reference);
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  protected boolean isFinished() {
    return Robot.m_wrist.atReference();
  }
}
