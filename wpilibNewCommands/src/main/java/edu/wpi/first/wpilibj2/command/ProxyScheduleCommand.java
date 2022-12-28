// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.Set;

/**
 * Schedules the given commands when this command is initialized, and ends when all the commands are
 * no longer scheduled. Useful for forking off from CommandGroups. If this command is interrupted,
 * it will cancel all of the commands.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class ProxyScheduleCommand extends CommandBase {
  private final Set<Command> m_toSchedule;
  private boolean m_finished;

  /**
   * Creates a new ProxyScheduleCommand that schedules the given commands when initialized, and ends
   * when they are all no longer scheduled.
   *
   * @param toSchedule the commands to schedule
   * @deprecated Replace with {@link ProxyCommand}, composing multiple of them in a {@link
   *     ParallelRaceGroup} if needed.
   */
  @Deprecated
  public ProxyScheduleCommand(Command... toSchedule) {
    m_toSchedule = Set.of(toSchedule);
  }

  @Override
  public void initialize() {
    for (Command command : m_toSchedule) {
      command.schedule();
    }
  }

  @Override
  public void end(boolean interrupted) {
    if (interrupted) {
      for (Command command : m_toSchedule) {
        command.cancel();
      }
    }
  }

  @Override
  public void execute() {
    m_finished = true;
    for (Command command : m_toSchedule) {
      m_finished &= !command.isScheduled();
    }
  }

  @Override
  public boolean isFinished() {
    return m_finished;
  }
}
