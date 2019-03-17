package edu.wpi.first.wpilibj.experimental.command;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import edu.wpi.first.wpilibj.command.IllegalUseOfCommandException;

/**
 * A CommandGroup that runs a set of commands in parallel, ending when the first command ends
 * and interrupting all the others.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 */
public class ParallelCommandRace extends CommandGroupBase {

  private final Set<Command> m_commands = new HashSet<>();
  private boolean m_runWhenDisabled = true;
  private boolean m_finished;

  /**
   * Creates a new ParallelCommandRace.  The given commands will be executed simultaneously, and
   * will "race to the finish" - the first command to finish ends the entire command, with all other
   * commands being interrupted.
   *
   * @param commands the commands to include in this group.
   */
  public ParallelCommandRace(Command... commands) {
    addCommands(commands);
  }

  @Override
  public void addCommands(Command... commands) {
    if (!Collections.disjoint(Set.of(commands), getGroupedCommands())) {
      throw new IllegalUseOfCommandException("Commands cannot be added to multiple CommandGroups");
    }

    registerGroupedCommands(commands);

    for (Command command : commands) {
      if (!Collections.disjoint(command.getRequirements(), m_requirements)) {
        throw new IllegalUseOfCommandException("Multiple commands in a parallel group cannot"
            + "require the same subsystems");
      }
      m_commands.add(command);
      m_requirements.addAll(command.getRequirements());
      m_runWhenDisabled &= command.runsWhenDisabled();
    }
  }

  @Override
  public void initialize() {
    for (Command command : m_commands) {
      command.initialize();
    }
  }

  @Override
  public void execute() {
    for (Command command : m_commands) {
      command.execute();
      if (command.isFinished()) {
        m_finished = true;
        command.end();
      }
    }
  }

  @Override
  public void interrupted() {
    for (Command command : m_commands) {
      command.interrupted();
    }
  }

  @Override
  public void end() {
    for (Command command : m_commands) {
      if (!command.isFinished()) {
        command.interrupted();
      }
    }
  }

  @Override
  public boolean isFinished() {
    return m_finished;
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
