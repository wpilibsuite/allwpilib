/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * A command that does nothing but takes a specified amount of time to finish.  Useful for
 * CommandGroups.  Can also be subclassed to make a command with an internal timer.
 */
public class WaitCommand extends CommandBase {
  protected Timer m_timer = new Timer();
  private final double m_duration;

  /**
   * Creates a new WaitCommand.  This command will do nothing, and end after the specified duration.
   *
   * @param seconds the time to wait, in seconds
   */
  public WaitCommand(double seconds) {
    m_duration = seconds;
    SendableRegistry.setName(this, getName() + ": " + seconds + " seconds");
  }

  @Override
  public void initialize() {
    m_timer.reset();
    m_timer.start();
  }

  @Override
  public void end(boolean interrupted) {
    m_timer.stop();
  }

  @Override
  public boolean isFinished() {
    return m_timer.hasElapsed(m_duration);
  }

  @Override
  public boolean runsWhenDisabled() {
    return true;
  }
}
