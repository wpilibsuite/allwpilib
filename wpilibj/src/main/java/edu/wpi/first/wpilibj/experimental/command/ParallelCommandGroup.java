package edu.wpi.first.wpilibj.experimental.command;

import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import edu.wpi.first.wpilibj.command.IllegalUseOfCommandException;

public class ParallelCommandGroup extends CommandGroupBase implements Command {

  //maps commands in this group to whether they are still running
  private final Map<Command, Boolean> m_commands = new HashMap<>();

  /**
   * Creates a new ParallelCommandGroup.  The given commands will be executed simultaneously.
   * The command group will finish when the last command finishes.  If the commandgroup is
   * interrupted, only the commands that are still running will be interrupted.
   *
   * @param commands the commands to include in this group.
   */
  public ParallelCommandGroup(Command... commands) {
    if (!Collections.disjoint(Set.of(commands), getGroupedCommands())) {
      throw new IllegalUseOfCommandException("Commands cannot be added to multiple CommandGroups");
    }

    registerGroupedCommands(commands);

    for (Command command : commands) {
      m_commands.put(command, true);
    }
  }

  @Override
  public void initialize() {
    for (Command command : m_commands.keySet()) {
      command.initialize();
      m_commands.replace(command, true);
    }
  }

  @Override
  public void execute() {
    for (Command command : m_commands.keySet()) {
      if (!m_commands.get(command)) {
        continue;
      }
      command.execute();
      if (command.isFinished()) {
        command.end();
        m_commands.replace(command, false);
      }
    }
  }

  @Override
  public void interrupted() {
    for (Command command : m_commands.keySet()) {
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
  public Set<Subsystem> getRequirements() {
    Set<Subsystem> requirements = new HashSet<>();
    for (Command command : m_commands.keySet()) {
      requirements.addAll(command.getRequirements());
    }
    return requirements;
  }

  @Override
  public boolean getRunWhenDisabled() {
    boolean allRunWhenDisabled = true;
    for (Command command : m_commands.keySet()) {
      allRunWhenDisabled &= command.getRunWhenDisabled();
    }
    return allRunWhenDisabled;
  }
}
