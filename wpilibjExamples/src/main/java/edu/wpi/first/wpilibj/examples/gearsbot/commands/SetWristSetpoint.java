/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.Command;

import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

/**
 * Move the wrist to a given angle. This command finishes when it is within the
 * tolerance, but leaves the PID loop running to maintain the position. Other
 * commands using the wrist should make sure they disable PID!
 */
public class SetWristSetpoint extends Command {
  private final double m_setpoint;

  public SetWristSetpoint(double setpoint) {
    m_setpoint = setpoint;
    requires(Robot.m_wrist);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    Robot.m_wrist.enable();
    Robot.m_wrist.setSetpoint(m_setpoint);
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  protected boolean isFinished() {
    return Robot.m_wrist.onTarget();
  }
}
