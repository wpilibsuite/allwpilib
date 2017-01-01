/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * A {@link TimedCommand} will wait for a timeout before finishing.
 * {@link TimedCommand} is used to execute a command for a given amount of time.
 */
public class TimedCommand extends Command {
  public TimedCommand(String name, double timeout) {
    super(name, timeout);
  }

  public TimedCommand(double timeout) {
    super(timeout);
  }

  protected boolean isFinished() {
    return isTimedOut();
  }
}
