// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.tunable.ComplexTunable;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableOption;
import org.wpilib.tunable.TunableTable;

/** A wrapper that allows a command to be published with the {@link ComplexTunable} interface. */
final class CommandComplexTunable implements ComplexTunable {

  private final Command m_command;

  /**
   * Creates a new CommandComplexTunable.
   *
   * @param command The command to be publishable
   */
  CommandComplexTunable(Command command) {
    requireNonNullParam(command, "command", "CommandComplexTunable");
    m_command = command;
  }

  /**
   * Finds the scheduler that owns the command requirements. If the command doesn't have any, gets
   * the default scheduler.
   *
   * @return A scheduler object
   */
  private Scheduler scheduler() {
    return m_command.requirements().stream()
        .findFirst()
        .map(Mechanism::getRegisteredScheduler)
        .orElseGet(Scheduler::getDefault);
  }

  @Override
  public void publishTunable(TunableTable table) {
    // Borrows the publish logic of command V2
    Scheduler scheduler = scheduler();

    table.publishValue(
        "name",
        m_command::name,
        null,
        String.class,
        TunableConfig.of(TunableOption.IMMUTABLE, TunableOption.GET_ON_CHANGE));

    table.publishBoolean(
        "running",
        () -> scheduler.isScheduledOrRunning(m_command),
        value -> {
          boolean isScheduled = scheduler.isScheduledOrRunning(m_command);
          if (value) {
            if (!isScheduled) {
              scheduler.schedule(m_command);
            }
          } else {
            if (isScheduled) {
              scheduler.cancel(m_command);
            }
          }
        },
        TunableConfig.of(TunableOption.ALWAYS_GET));
  }

  @Override
  public String getTunableType() {
    return "Command";
  }
}
