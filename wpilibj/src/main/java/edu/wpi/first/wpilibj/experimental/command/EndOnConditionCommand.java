package edu.wpi.first.wpilibj.experimental.command;


import java.util.function.BooleanSupplier;

/**
 * A command that does nothing, and ends when a specified condition is true.  Useful for
 * CommandGroups.
 */
public class EndOnConditionCommand extends SendableCommandBase {

  private final BooleanSupplier m_condition;

  /**
   * Creates a new EndOnConditionCommand.
   *
   * @param condition the condition to determine when to end
   */
  public EndOnConditionCommand(BooleanSupplier condition) {
    m_condition = condition;
  }

  @Override
  public boolean isFinished() {
    return m_condition.getAsBoolean();
  }

  @Override
  public boolean runsWhenDisabled() {
    return true;
  }
}
