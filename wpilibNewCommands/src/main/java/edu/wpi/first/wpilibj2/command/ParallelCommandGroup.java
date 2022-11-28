// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * A CommandGroup that runs a set of commands in parallel, ending when the last command ends.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class ParallelCommandGroup extends CommandGroupBase {
  // maps commands in this group to whether they are still running
  private final Map<Command, Boolean> m_commands = new HashMap<>();
  private boolean m_runWhenDisabled = true;
  private InterruptionBehavior m_interruptBehavior = InterruptionBehavior.kCancelIncoming;

  /**
   * Creates a new ParallelCommandGroup. The given commands will be executed simultaneously. The
   * command group will finish when the last command finishes. If the CommandGroup is interrupted,
   * only the commands that are still running will be interrupted.
   *
   * @param commands the commands to include in this group.
   */
  public ParallelCommandGroup(Command... commands) {
    addCommands(commands);
  }

  @Override
  public final void addCommands(Command... commands) {
    requireUngrouped(commands);

    if (m_commands.containsValue(true)) {
      throw new IllegalStateException(
          "Commands cannot be added to a CommandGroup while the group is running");
    }

    registerGroupedCommands(commands);

    for (Command command : commands) {
      if (!Collections.disjoint(command.getRequirements(), m_requirements)) {
        throw new IllegalArgumentException(
            "Multiple commands in a parallel group cannot" + "require the same subsystems");
      }
      m_commands.put(command, false);
      m_requirements.addAll(command.getRequirements());
      m_runWhenDisabled &= command.runsWhenDisabled();
      if (command.getInterruptionBehavior() == InterruptionBehavior.kCancelSelf) {
        m_interruptBehavior = InterruptionBehavior.kCancelSelf;
      }
    }
  }

  @Override
  public final void initialize() {
    for (Map.Entry<Command, Boolean> commandRunning : m_commands.entrySet()) {
      commandRunning.getKey().initialize();
      commandRunning.setValue(true);
    }
  }

  @Override
  public final void execute() {
    for (Map.Entry<Command, Boolean> commandRunning : m_commands.entrySet()) {
      if (!commandRunning.getValue()) {
        continue;
      }
      commandRunning.getKey().execute();
      if (commandRunning.getKey().isFinished()) {
        commandRunning.getKey().end(false);
        commandRunning.setValue(false);
      }
    }
  }

  @Override
  public final void end(boolean interrupted) {
    if (interrupted) {
      for (Map.Entry<Command, Boolean> commandRunning : m_commands.entrySet()) {
        if (commandRunning.getValue()) {
          commandRunning.getKey().end(true);
        }
      }
    }
  }

  @Override
  public final boolean isFinished() {
    return !m_commands.containsValue(true);
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
