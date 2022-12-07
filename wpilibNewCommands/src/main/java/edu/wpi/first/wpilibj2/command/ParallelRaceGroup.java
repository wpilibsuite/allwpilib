// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

/**
 * A CommandGroup that runs a set of commands in parallel, ending when any one of the commands ends
 * and interrupting all the others.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class ParallelRaceGroup extends CommandGroupBase {
  private final Set<Command> m_commands = new HashSet<>();
  private boolean m_runWhenDisabled = true;
  private boolean m_finished = true;
  private InterruptionBehavior m_interruptBehavior = InterruptionBehavior.kCancelIncoming;

  /**
   * Creates a new ParallelCommandRace. The given commands will be executed simultaneously, and will
   * "race to the finish" - the first command to finish ends the entire command, with all other
   * commands being interrupted.
   *
   * @param commands the commands to include in this group.
   */
  public ParallelRaceGroup(Command... commands) {
    addCommands(commands);
  }

  @Override
  public final void addCommands(Command... commands) {
    requireUngrouped(commands);

    if (!m_finished) {
      throw new IllegalStateException(
          "Commands cannot be added to a CommandGroup while the group is running");
    }

    registerGroupedCommands(commands);

    for (Command command : commands) {
      if (!Collections.disjoint(command.getRequirements(), m_requirements)) {
        throw new IllegalArgumentException(
            "Multiple commands in a parallel group cannot" + " require the same subsystems");
      }
      m_commands.add(command);
      m_requirements.addAll(command.getRequirements());
      m_runWhenDisabled &= command.runsWhenDisabled();
      if (command.getInterruptionBehavior() == InterruptionBehavior.kCancelSelf) {
        m_interruptBehavior = InterruptionBehavior.kCancelSelf;
      }
    }
  }

  @Override
  public final void initialize() {
    m_finished = false;
    for (Command command : m_commands) {
      command.initialize();
    }
  }

  @Override
  public final void execute() {
    for (Command command : m_commands) {
      command.execute();
      if (command.isFinished()) {
        m_finished = true;
      }
    }
  }

  @Override
  public final void end(boolean interrupted) {
    for (Command command : m_commands) {
      command.end(!command.isFinished());
    }
  }

  @Override
  public final boolean isFinished() {
    return m_finished;
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_runWhenDisabled;
  }

  @Override
  public InterruptionBehavior getInterruptionBehavior() {
    return m_interruptBehavior;
  }
}
