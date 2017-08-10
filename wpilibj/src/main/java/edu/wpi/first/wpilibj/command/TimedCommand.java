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
  /**
   * Instantiates a TimedCommand with the given name and timeout.
   *
   * @param name the name of the command
   * @param timeout the time the command takes to run (seconds)
   */
  public TimedCommand(String name, double timeout) {
    super(name, timeout);
  }

  /**
   * Instantiates a TimedCommand with the given timeout.
   *
   * @param timeout the time the command takes to run (seconds)
   */
  public TimedCommand(double timeout) {
    super(timeout);
  }

  /**
  * Ends command when timed out.
  */
  protected boolean isFinished() {
    return isTimedOut();
  }
}
