package edu.wpi.first.wpilibj.experimental.command;

public class ScheduleCommand implements Command {

  private final Command m_toSchedule;

  /**
   * Schedules the given command when this command is initialized.  Useful for forking off from
   * CommandGroups.  Note that if run from a CommandGroup, the group will not know about the status
   * of the scheduled command, and will treat this command as finishing instantly.
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
  public void execute() {

  }

  @Override
  public void end() {

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
