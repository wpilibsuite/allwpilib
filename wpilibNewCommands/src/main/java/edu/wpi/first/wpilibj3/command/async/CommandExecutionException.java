package edu.wpi.first.wpilibj3.command.async;

/**
 * An exception thrown when an {@link AsyncCommand} encountered an error while running in an {@link
 * AsyncScheduler}.
 */
public class CommandExecutionException extends RuntimeException {
  /** The command that had the exception. */
  private final AsyncCommand command;

  /**
   * Creates a new CommandExecutionException.
   *
   * @param command the command that encountered the exception
   * @param cause the exception itself
   */
  public CommandExecutionException(AsyncCommand command, Throwable cause) {
    super("An exception was detected while running command " + command, cause);
    this.command = command;
  }

  /**
   * Gets the command that encountered the exception.
   *
   * @return the command
   */
  public AsyncCommand getCommand() {
    return command;
  }
}
