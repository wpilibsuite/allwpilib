// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;
import edu.wpi.first.wpilibj2.command.CommandBase;

/**
 * Move the wrist to a given angle. This command finishes when it is within the tolerance, but
 * leaves the PID loop running to maintain the position. Other commands using the wrist should make
 * sure they disable PID!
 */
public class SetWristSetpoint extends CommandBase {
  private final Wrist m_wrist;
  private final double m_setpoint;

  /**
   * Create a new SetWristSetpoint command.
   *
   * @param setpoint The setpoint to set the wrist to
   * @param wrist The wrist to use
   */
  public SetWristSetpoint(double setpoint, Wrist wrist) {
    m_wrist = wrist;
    m_setpoint = setpoint;
    addRequirements(m_wrist);
  }

  // Called just before this Command runs the first time
  @Override
  public void initialize() {
    m_wrist.enable();
    m_wrist.setSetpoint(m_setpoint);
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  public boolean isFinished() {
    return m_wrist.getController().atSetpoint();
  }
}
