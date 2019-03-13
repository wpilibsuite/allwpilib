package edu.wpi.first.wpilibj.command;


import java.util.*;
import java.util.Set;

public class ParallelCommandGroup extends CommandGroupBase implements ICommand {

  private final Map<ICommand, Boolean> m_commands = new HashMap<>();

  public ParallelCommandGroup(ICommand[] commands) {
    if (!Collections.disjoint(Set.of(commands), getGroupedCommands())) {
      throw new IllegalUseOfCommandException("Commands cannot be added to more than one CommandGroup");
    }

    registerGroupedCommands(commands);

    for (ICommand command : commands) {
      m_commands.put(command, true);
    }
  }

  @Override
  public void initialize() {
    for (ICommand command : m_commands.keySet()) {
      command.initialize();
      m_commands.replace(command, true);
    }
  }

  @Override
  public void execute() {
    for (ICommand command : m_commands.keySet()) {
      if (command.isFinished()) {
        command.end();
        m_commands.replace(command, false);
      } else {
        command.execute();
      }
    }
  }

  @Override
  public void interrupted() {
    for (ICommand command : m_commands.keySet()) {
      if (m_commands.get(command)) {
        command.interrupted();
      }
    }
  }

  @Override
  public void end() {
  }

  @Override
  public boolean isFinished() {
    return !m_commands.values().contains(true);
  }

  @Override
  public Collection<Subsystem> getRequirements() {
    Collection<Subsystem> requirements = new HashSet<>();
    for (ICommand command : m_commands.keySet()) {
      requirements.addAll(command.getRequirements());
    }
    return requirements;
  }
}
