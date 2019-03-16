package edu.wpi.first.wpilibj.experimental.command;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import edu.wpi.first.wpilibj.command.IllegalUseOfCommandException;

public class SequentialCommandGroup extends CommandGroupBase implements Command {

  private final List<Command> m_commands = new ArrayList<>();
  private final Set<Subsystem> m_requirements = new HashSet<>();
  private int m_currentCommandIndex;
  private boolean m_runWhenDisabled = true;

  /**
   * Creates a new SequentialCommandGroup.  The given commands will be run sequentially, with
   * the commandgroup finishing when the last command finishes.
   *
   * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
   *
   * @param commands the commands to include in this group.
   */
  public SequentialCommandGroup(Command... commands) {
    addCommands(commands);
  }

  @Override
  public void addCommands(Command... commands) {
    if (!Collections.disjoint(Set.of(commands), getGroupedCommands())) {
      throw new IllegalUseOfCommandException(
          "Commands cannot be added to more than one CommandGroup");
    }

    registerGroupedCommands(commands);

    for (Command command : commands) {
      m_commands.add(command);
      m_requirements.addAll(command.getRequirements());
      m_runWhenDisabled &= command.runsWhenDisabled();
    }
  }

  @Override
  public void initialize() {
    m_currentCommandIndex = 0;

    if (!m_commands.isEmpty()) {
      m_commands.get(0).initialize();
    }
  }

  @Override
  public void execute() {

    if (m_commands.isEmpty()) {
      return;
    }

    Command currentCommand = m_commands.get(m_currentCommandIndex);

    currentCommand.execute();
    if (currentCommand.isFinished()) {
      currentCommand.end();
      m_currentCommandIndex++;
      if (m_currentCommandIndex < m_commands.size()) {
        m_commands.get(m_currentCommandIndex).initialize();
      }
    }
  }

  @Override
  public void interrupted() {
    if (!m_commands.isEmpty()) {
      m_commands.get(m_currentCommandIndex).interrupted();
    }
  }

  @Override
  public void end() {
  }

  @Override
  public boolean isFinished() {
    return m_currentCommandIndex == m_commands.size();
  }

  @Override
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_runWhenDisabled;
  }
}
