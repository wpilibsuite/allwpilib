package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;

/**
 * Schedules the given commands with this command is initialized, and ends when all the commands are
 * no longer scheduled.  Useful for forking off from CommandGroups.
 */
public class BlockingScheduleCommand extends SendableCommandBase {

  private final Set<Command> m_toSchedule;
  private boolean finished;

  /**
   * Creates a new BlockingScheduleCommand that schedules the given commands when initialized,
   * and ends when they are all no longer scheduled.
   *
   * @param toSchedule the commands to schedule
   */
  public BlockingScheduleCommand(Command... toSchedule) {
    m_toSchedule = Set.of(toSchedule);
  }

  @Override
  public void initialize() {
    finished = false;
    for (Command command : m_toSchedule) {
      command.schedule();
    }
  }

  @Override
  public void execute() {
    for (Command command : m_toSchedule) {
      finished &= command.isScheduled();
    }
  }

  @Override
  public boolean isFinished() {
    return finished;
  }
}
