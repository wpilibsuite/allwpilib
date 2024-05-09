// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * A command composition that runs a set of commands in parallel, ending when the last command ends.
 *
 * <p>The rules for command compositions apply: command instances that are passed to it cannot be
 * added to any other composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class ParallelCommandGroup extends Command {
  private final List<Command> m_commands = new ArrayList<>();
  private final List<Boolean> m_commandsAreRunning = new ArrayList<>();
  private boolean m_runWhenDisabled = true;
  private InterruptionBehavior m_interruptBehavior = InterruptionBehavior.kCancelIncoming;

  /**
   * Creates a new ParallelCommandGroup. The given commands will be executed simultaneously. The
   * command composition will finish when the last command finishes. If the composition is
   * interrupted, only the commands that are still running will be interrupted.
   *
   * @param commands the commands to include in this composition.
   */
  public ParallelCommandGroup(Command... commands) {
    addCommands(commands);
  }

  /**
   * Adds the given commands to the group.
   *
   * @param commands Commands to add to the group.
   */
  public final void addCommands(Command... commands) {
    if (m_commandsAreRunning.contains(true)) {
      throw new IllegalStateException(
          "Commands cannot be added to a composition while it's running");
    }

    CommandScheduler.getInstance().registerComposedCommands(commands);

    for (Command command : commands) {
      if (!Collections.disjoint(command.getRequirements(), m_requirements)) {
        throw new IllegalArgumentException(
            "Multiple commands in a parallel composition cannot require the same subsystems");
      }
      m_commands.add(command);
      m_commandsAreRunning.add(false);
      m_requirements.addAll(command.getRequirements());
      m_runWhenDisabled &= command.runsWhenDisabled();
      if (command.getInterruptionBehavior() == InterruptionBehavior.kCancelSelf) {
        m_interruptBehavior = InterruptionBehavior.kCancelSelf;
      }
    }
  }

  @Override
  public final void initialize() {
    for (int i = 0; i < m_commands.size(); ++i) {
      m_commands.get(i).initialize();
      m_commandsAreRunning.set(i, true);
    }
  }

  @Override
  public final void execute() {
    for (int i = 0; i < m_commands.size(); ++i) {
      if (!m_commandsAreRunning.get(i)) {
        continue;
      }
      m_commands.get(i).execute();
      if (m_commands.get(i).isFinished()) {
        m_commands.get(i).end(false);
        m_commandsAreRunning.set(i, false);
      }
    }
  }

  @Override
  public final void end(boolean interrupted) {
    if (interrupted) {
      for (int i = 0; i < m_commands.size(); ++i) {
        if (m_commandsAreRunning.get(i)) {
          m_commands.get(i).end(true);
        }
      }
    }
  }

  @Override
  public final boolean isFinished() {
    return !m_commandsAreRunning.contains(true);
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
