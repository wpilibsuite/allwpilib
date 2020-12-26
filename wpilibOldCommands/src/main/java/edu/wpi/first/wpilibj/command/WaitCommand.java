// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
