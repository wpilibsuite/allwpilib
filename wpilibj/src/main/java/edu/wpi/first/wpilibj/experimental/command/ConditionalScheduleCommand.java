package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.BooleanSupplier;

/**
 * Schedules one of two commands when this command is initialized, depending on the value of the
 * given condition.  Note that if this is run from a CommandGroup, the group will not
 * know about the status of the scheduled command, and will treat this command as finishing
 * instantly.  If this is not desired, consider using {@link ConditionalCommand}.
 */
public class ConditionalScheduleCommand implements Command {

  private final Command m_onTrue;
  private final Command m_onFalse;
  private final BooleanSupplier m_condition;
  private final boolean m_interruptible;

  /**
   * Creates a new ConditionalScheduleCommand.
   *
   * @param onTrue        the command to schedule if the condition is true
   * @param onFalse       the command to schedule if the condition is false
   * @param condition     the condition to determine which command to schedule
   * @param interruptible whether the scheduled command should be interruptible
   */
  public ConditionalScheduleCommand(Command onTrue, Command onFalse, BooleanSupplier condition,
                                    boolean interruptible) {
    m_onTrue = onTrue;
    m_onFalse = onFalse;
    m_condition = condition;
    m_interruptible = interruptible;
  }

  /**
   * Creates a new ConditionalScheduleCommand.  The scheduled command will be interruptible.
   *
   * @param onTrue    the command to schedule if the condition is true
   * @param onFalse   the command to schedule if the condition is false
   * @param condition the condition to determine which command to schedule
   */
  public ConditionalScheduleCommand(Command onTrue, Command onFalse, BooleanSupplier condition) {
    this(onTrue, onFalse, condition, true);
  }

  @Override
  public void initialize() {
    if (m_condition.getAsBoolean()) {
      m_onTrue.schedule(m_interruptible);
    } else {
      m_onFalse.schedule(m_interruptible);
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
