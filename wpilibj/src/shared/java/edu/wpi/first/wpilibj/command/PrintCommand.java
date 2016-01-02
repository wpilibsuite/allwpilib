/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * A {@link PrintCommand} is a command which prints out a string when it is
 * initialized, and then immediately finishes. It is useful if you want a
 * {@link CommandGroup} to print out a string when it reaches a certain point.
 *
 * @author Joe Grinstead
 */
public class PrintCommand extends Command {

  /** The message to print out */
  private String message;

  /**
   * Instantiates a {@link PrintCommand} which will print the given message when
   * it is run.
   *$
   * @param message the message to print
   */
  public PrintCommand(String message) {
    super("Print(\"" + message + "\"");
    this.message = message;
  }

  protected void initialize() {
    System.out.println(message);
  }

  protected void execute() {}

  protected boolean isFinished() {
    return true;
  }

  protected void end() {}

  protected void interrupted() {}
}
