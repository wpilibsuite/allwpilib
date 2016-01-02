/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import edu.wpi.first.wpilibj.Timer;

/**
 * WaitUntilCommand - waits until an absolute game time. This will wait until
 * the game clock reaches some value, then continue to the next command.
 *$
 * @author brad
 *
 */
public class WaitUntilCommand extends Command {

  private double m_time;

  public WaitUntilCommand(double time) {
    super("WaitUntil(" + time + ")");
    m_time = time;
  }

  public void initialize() {}

  public void execute() {}

  /**
   * Check if we've reached the actual finish time.
   */
  public boolean isFinished() {
    return Timer.getMatchTime() >= m_time;
  }

  public void end() {}

  public void interrupted() {}
}
