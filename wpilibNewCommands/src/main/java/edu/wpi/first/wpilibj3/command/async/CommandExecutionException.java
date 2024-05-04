package edu.wpi.first.wpilibj3.command.async;

public class CommandExecutionException extends RuntimeException {
  private final AsyncCommand command;

  public CommandExecutionException(AsyncCommand command, Throwable cause) {
    super("An exception was detected while running command " + command, cause);
    this.command = command;
  }

  public AsyncCommand getCommand() {
    return command;
  }
}
