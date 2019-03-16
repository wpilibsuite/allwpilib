package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.BooleanSupplier;

public class ConditionalScheduleCommand implements Command {

  private final Command m_onTrue;
  private final Command m_onFalse;
  private final BooleanSupplier m_condition;

  /**
   * Schedules one of two commands when this command is initialized, depending on the value of the
   * given condition.  Note that if this is run from a CommandGroup, the group will not
   * know about the status of the scheduled command, and will treat this command as finishing
   * instantly.  If this is not desired, consider using {@link ConditionalCommand}.
   *
   * @param onTrue    the command to schedule if the condition is true
   * @param onFalse   the command to schedule if the condition is false
   * @param condition the condition to determine which command to schedule
   */
  public ConditionalScheduleCommand(Command onTrue, Command onFalse, BooleanSupplier condition) {
    m_onTrue = onTrue;
    m_onFalse = onFalse;
    m_condition = condition;
  }

  @Override
  public void initialize() {
    if (m_condition.getAsBoolean()) {
      m_onTrue.schedule();
    } else {
      m_onFalse.schedule();
    }
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
}
