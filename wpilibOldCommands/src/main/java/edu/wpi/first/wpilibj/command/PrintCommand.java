// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

/**
 * A {@link PrintCommand} is a command which prints out a string when it is initialized, and then
 * immediately finishes. It is useful if you want a {@link CommandGroup} to print out a string when
 * it reaches a certain point.
 *
 * <p>This class is provided by the OldCommands VendorDep
 */
public class PrintCommand extends InstantCommand {
  /** The message to print out. */
  private final String m_message;

  /**
   * Instantiates a {@link PrintCommand} which will print the given message when it is run.
   *
   * @param message the message to print
   */
  public PrintCommand(String message) {
    super("Print(\"" + message + "\"");
    m_message = message;
  }

  @Override
  protected void initialize() {
    System.out.println(m_message);
  }
}
