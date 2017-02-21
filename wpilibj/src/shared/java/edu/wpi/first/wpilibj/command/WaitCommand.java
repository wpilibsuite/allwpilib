/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * A {@link WaitCommand} will wait for a certain amount of time before finishing. It is useful if
 * you want a {@link CommandGroup} to pause for a moment.
 *
 * @see CommandGroup
 */
public class WaitCommand extends TimedCommand {

  /**
   * Instantiates a {@link WaitCommand} with the given timeout.
   *
   * @param timeout the time the command takes to run (seconds)
   */
  public WaitCommand(double timeout) {
    this("Wait(" + timeout + ")", timeout);
  }

  /**
   * Instantiates a {@link WaitCommand} with the given timeout.
   *
   * @param name    the name of the command
   * @param timeout the time the command takes to run (seconds)
   */
  public WaitCommand(String name, double timeout) {
    super(name, timeout);
  }
}
