package edu.wpi.first.wpilibj.experimental.command;

import java.util.Collections;
import java.util.Map;
import java.util.Set;
import java.util.function.Supplier;

import edu.wpi.first.wpilibj.command.IllegalUseOfCommandException;

/**
 * Runs one of a selection of commands, either using a selector and a key->command mapping, or a
 * supplier that returns the command directly at runtime.  Does not actually schedule the selected
 * command - rather, the command is run through this command; this ensures that the command will
 * behave as expected if used as part of a CommandGroup.  Requires the requirements of all included
 * commands, again to ensure proper functioning when used in a CommandGroup.  If this is undesired,
 * consider using {@link ScheduleCommand}.
 *
 * <p>As this command contains multiple component commands within it, it is technically a command
 * group; the command instances that are passed to it cannot be added to any other groups, or
 * scheduled individually.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 *
 * @param <K> the type of the selector to use, e.g. String or enum
 */
public class SelectCommand extends SendableCommandBase {

  private final Map<Object, Command> m_commands;
  private final Supplier<Object> m_selector;
  private final Supplier<Command> m_toRun;
  private Command m_selectedCommand;

  /**
   * Creates a new SelectCommand.
   *
   * @param commands the map of commands to choose from
   * @param selector the selector to determine which command to run
   */
  public SelectCommand(Map<Object, Command> commands, Supplier<Object> selector) {
    if (!Collections.disjoint(CommandGroupBase.getGroupedCommands(), commands.values())) {
      throw new IllegalUseOfCommandException("Commands cannot be added to multiple CommandGroups");
    }

    CommandGroupBase.registerGroupedCommands(commands.values().toArray(new Command[]{}));

    m_commands = commands;
    m_selector = selector;
    m_toRun = null;

    for (Command command : m_commands.values()) {
      m_requirements.addAll(command.getRequirements());
    }
  }

  /**
   * Creates a new SelectCommand.
   *
   * @param toRun a supplier providing the command to run
   */
  public SelectCommand(Supplier<Command> toRun) {
    m_commands = null;
    m_selector = null;
    m_toRun = toRun;
  }

  @Override
  public void initialize() {
    m_selectedCommand = (m_selector != null) ? m_commands.get(m_selector.get()) : m_toRun.get();
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
