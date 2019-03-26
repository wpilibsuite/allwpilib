package edu.wpi.first.wpilibj.experimental.command;

/**
 * A command that runs another command in perpetuity, ignoring that command's end conditions.
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
