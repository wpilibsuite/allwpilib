package edu.wpi.first.wpilibj.command;

/**
 * Useful helper methods to make robot program automated testing of commands easier.
 */
public final class CommandTestHelper {
  /**
   * Static class. Do not instantiate.
   */
  private CommandTestHelper() {}

  /**
   * Expose the doesRequire method so that we can test whether the appropriate
   * subsystems are required within a command.
   * 
   * @param command       The command to check
   * @param subsystem     The subsystem to check
   * @return              True if the subsystem was required by the command
   */
  public static boolean doesRequire(Command command, Subsystem subsystem) {
    return command.doesRequire(subsystem);
  }
}
