/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * This command will execute once, then finish immediately afterward.
 *
 * <p>Subclassing {@link InstantCommand} is shorthand for returning true from
 * {@link Command isFinished}.
 */
public class InstantCommand extends Command {

  public InstantCommand() {
  }

  /**
   * Creates a new {@link InstantCommand InstantCommand} with the given name.
   * @param name the name for this command
   */
  public InstantCommand(String name) {
    super(name);
  }

  protected boolean isFinished() {
    return true;
  }
}
