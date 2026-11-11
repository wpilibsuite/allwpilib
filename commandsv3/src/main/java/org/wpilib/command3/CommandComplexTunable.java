// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.tunable.ComplexTunable;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableOption;
import org.wpilib.tunable.TunableTable;

/**
 * A wrapper that allows a command to be published with the {@link ComplexTunable} interface.
 */
final class CommandComplexTunable implements ComplexTunable {
  private final Scheduler m_scheduler;
  private final Command m_command;

  /**
   * Creates a new CommandComplexTunable
   * 
   * @param scheduler The scheduler responsible of running the command
   * @param command The command to be published
   */
  CommandComplexTunable(Scheduler scheduler, Command command) {
    requireNonNullParam(scheduler, "scheduler", "CommandComplexTunable");
    requireNonNullParam(command, "command", "CommandComplexTunable");
    m_scheduler = scheduler;
    m_command = command;
  }

  @Override
  public void publishTunable(TunableTable table) {
    // Borrows the logic of command V2
    table.publishValue("name", m_command::name, null, String.class,
        TunableConfig.of(TunableOption.IMMUTABLE, TunableOption.GET_ON_CHANGE));

    table.publishBoolean("running", () -> m_scheduler.isScheduledOrRunning(m_command), value -> {
      boolean isScheduled = m_scheduler.isScheduledOrRunning(m_command);
      if (value) {
        if (!isScheduled) {
          m_scheduler.schedule(m_command);
        }
      } else {
        if (isScheduled) {
          m_scheduler.cancel(m_command);
        }
      }
    }, TunableConfig.of(TunableOption.ALWAYS_GET));
  }

  @Override
  public String getTunableType() {
    return "Command";
  }
}
