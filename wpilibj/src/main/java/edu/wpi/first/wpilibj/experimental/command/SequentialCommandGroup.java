package edu.wpi.first.wpilibj.experimental.command;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Queue;
import java.util.Set;

import edu.wpi.first.wpilibj.command.IllegalUseOfCommandException;

public class SequentialCommandGroup extends CommandGroupBase implements Command {

  private final List<Command> m_commands = new ArrayList<>();
  private Queue<Command> m_commandQueue;

  /**
   * Creates a new SequentialCommandGroup.  The given commands will be run sequentially, with
   * the commandgroup finishing when the last command finishes.
   *
   * @param commands the commands to include in this group.
   */
  public SequentialCommandGroup(Command... commands) {
    if (!Collections.disjoint(Set.of(commands), getGroupedCommands())) {
      throw new IllegalUseOfCommandException(
          "Commands cannot be added to more than one CommandGroup");
    }

    registerGroupedCommands(commands);

    m_commands.addAll(List.of(commands));
  }

  @Override
  public void initialize() {
    m_commandQueue = new ArrayDeque<>(m_commands);
    if (!m_commandQueue.isEmpty()) {
      m_commandQueue.peek().initialize();
    }
  }

  @Override
  public void execute() {

    if (m_commandQueue.isEmpty()) {
      return;
    }

    m_commandQueue.peek().execute();
    if (m_commandQueue.peek().isFinished()) {
      m_commandQueue.remove().end();
      if (!m_commandQueue.isEmpty()) {
        m_commandQueue.peek().initialize();
      }
    }
  }

  @Override
  public void interrupted() {
    if (!m_commandQueue.isEmpty()) {
      m_commandQueue.peek().interrupted();
    }
  }

  @Override
  public void end() {
  }

  @Override
  public boolean isFinished() {
    return m_commandQueue.isEmpty();
  }

  @Override
  public Collection<Subsystem> getRequirements() {
    Collection<Subsystem> requirements = new HashSet<>();
    for (Command command : m_commands) {
      requirements.addAll(command.getRequirements());
    }
    return requirements;
  }

  @Override
  public boolean getRunWhenDisabled() {
    boolean allRunWhenDisabled = true;
    for (Command command : m_commands) {
      allRunWhenDisabled &= command.getRunWhenDisabled();
    }
    return allRunWhenDisabled;
  }
}
