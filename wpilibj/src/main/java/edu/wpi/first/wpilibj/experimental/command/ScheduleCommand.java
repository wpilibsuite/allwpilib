package edu.wpi.first.wpilibj.experimental.command;

/**
 * Schedules a given command when this command is initialized.  Useful for forking off from
 * CommandGroups.  Note that if run from a CommandGroup, the group will not know about the status
 * of the scheduled command, and will treat this command as finishing instantly.
 */
public class ScheduleCommand extends SendableCommandBase {

  private final Command m_toSchedule;

  /**
   * Creates a new ScheduleCommand that schedules the given command when initialized.
   *
   * @param toSchedule the command to schedule
   */
  public ScheduleCommand(Command toSchedule) {
    m_toSchedule = toSchedule;
  }

  @Override
  public void initialize() {
    m_toSchedule.schedule();
  }

  @Override
  public boolean isFinished() {
    return true;
  }

  @Override
  public boolean runsWhenDisabled() {
    return true;
  }
}
