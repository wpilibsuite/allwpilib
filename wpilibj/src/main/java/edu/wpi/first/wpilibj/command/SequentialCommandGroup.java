package edu.wpi.first.wpilibj.command;


import java.util.*;
import java.util.Set;

public class SequentialCommandGroup extends CommandGroupBase implements ICommand {

  private final List<ICommand> m_commands = new ArrayList<>();
  private Queue<ICommand> m_commandQueue;

  public SequentialCommandGroup(ICommand[] commands) {
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
    if (!m_commandQueue.isEmpty()) {
      m_commandQueue.peek().execute();
      if (m_commandQueue.peek().isFinished()) {
        m_commandQueue.remove().end();
        if (!m_commandQueue.isEmpty()) {
          m_commandQueue.peek().initialize();
        }
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
    for (ICommand command : m_commands) {
      requirements.addAll(command.getRequirements());
    }
    return requirements;
  }
}
