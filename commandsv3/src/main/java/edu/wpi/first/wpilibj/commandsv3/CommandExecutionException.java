// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3;

/**
 * An exception thrown when a {@link Command} encounters an error while running in a {@link
 * Scheduler}.
 */
public class CommandExecutionException extends RuntimeException {
  /** The command that had the exception. */
  private final Command command;

  /**
   * Creates a new CommandExecutionException.
   *
   * @param command the command that encountered the exception
   * @param cause the exception itself
   */
  public CommandExecutionException(Command command, Throwable cause) {
    super("An exception was detected while running command " + command.name(), cause);
    this.command = command;
  }

  /**
   * Gets the command that encountered the exception.
   *
   * @return the command
   */
  public Command getCommand() {
    return command;
  }
}
