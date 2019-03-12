package edu.wpi.first.wpilibj.experimental.command;

/**
 * A command that runs another command in perpetuity, ignoring that command's end conditions.  While
 * this class does not extend {@link CommandGroupBase}, it is still considered a CommandGroup, as
 * it allows one to compose another command within it; that command is added to the list of grouped
 * commands, and the ordinary restrictions apply.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 */
public class PerpetualCommand extends SendableCommandBase {

  protected final Command m_command;

  /**
   * Creates a new PerpetualCommand.  Will run another command in perpetuity, ignoring that
   * command's end conditions, unless this command itself is interrupted.
   *
   * @param command the command to run perpetually
   */
  public PerpetualCommand(Command command) {
    m_command = command;
    m_requirements.addAll(command.getRequirements());
    CommandGroupBase.registerGroupedCommands(command);
  }

  @Override
  public void initialize() {
    m_command.initialize();
  }

  @Override
  public void execute() {
    m_command.execute();
  }

  @Override
  public void end(boolean interrupted) {
    m_command.end(interrupted);
  }
}
