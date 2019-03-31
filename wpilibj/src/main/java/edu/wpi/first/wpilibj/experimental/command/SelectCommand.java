package edu.wpi.first.wpilibj.experimental.command;

import java.util.Map;
import java.util.Set;
import java.util.function.Supplier;

/**
 * Runs one of a selection of commands, depending on the value of the selector when this command is
 * initialized.  Does not actually schedule the selected command - rather, the command is run
 * through this command; this ensures that the command will behave as expected if used as
 * part of a CommandGroup.  Requires the requirements of all included commands, again to ensure
 * proper functioning when used in a CommandGroup.  If this is undesired, consider using
 * {@link ScheduleCommand}.
 *
 * @param <K> the type of the selector to use, e.g. String or enum
 */
public class SelectCommand<K> extends SendableCommandBase {

  private final Map<K, Command> m_commands;
  private final Supplier<K> m_selector;
  private Command m_selectedCommand;

  /**
   * Creates a new selectcommand.
   *
   * @param commands the map of commands to choose from
   * @param selector the selector to determine which command to run
   */
  public SelectCommand(Map<K, Command> commands, Supplier<K> selector) {
    m_commands = commands;
    m_selector = selector;
    for (Command command : m_commands.values()) {
      m_requirements.addAll(command.getRequirements());
    }
  }

  @Override
  public void initialize() {
    m_selectedCommand = m_commands.get(m_selector.get());
    m_selectedCommand.initialize();
  }

  @Override
  public void execute() {
    m_selectedCommand.execute();
  }

  @Override
  public void end(boolean interrupted) {
    m_selectedCommand.end(interrupted);
  }

  @Override
  public boolean isFinished() {
    return m_selectedCommand.isFinished();
  }

  @Override
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_selectedCommand.runsWhenDisabled();
  }
}
