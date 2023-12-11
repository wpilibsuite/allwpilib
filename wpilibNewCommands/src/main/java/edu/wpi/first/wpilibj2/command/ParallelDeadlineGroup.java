// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.util.sendable.SendableBuilder;

/**
 * A command composition that runs a set of commands in parallel, ending only when a specific
 * command (the "deadline") ends, interrupting all other commands that are still running at that
 * point.
 *
 * <p>The rules for command compositions apply: command instances that are passed to it cannot be
 * added to any other composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class ParallelDeadlineGroup extends ParallelCommandGroup {
  private Command m_deadline;

  /**
   * Creates a new ParallelDeadlineGroup. The given commands (including the deadline) will be
   * executed simultaneously. The composition will finish when the deadline finishes, interrupting
   * all other still-running commands. If the composition is interrupted, only the commands still
   * running will be interrupted.
   *
   * @param deadline the command that determines when the composition ends
   * @param commands the commands to be executed
   */
  public ParallelDeadlineGroup(Command deadline, Command... commands) {
    super(commands);
    setDeadline(deadline);
  }

  /**
   * Sets the deadline to the given command. The deadline is added to the group if it is not already
   * contained.
   *
   * @param deadline the command that determines when the group ends
   */
  public void setDeadline(Command deadline) {
    if (!getCommandStatuses().containsKey(deadline)) {
      addCommands(deadline);
    }
    m_deadline = deadline;
  }

  @Override
  public final boolean isFinished() {
    return !getCommandStatuses().get(m_deadline);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    super.initSendable(builder);

    builder.addStringProperty("deadline", m_deadline::getName, null);
  }
}
