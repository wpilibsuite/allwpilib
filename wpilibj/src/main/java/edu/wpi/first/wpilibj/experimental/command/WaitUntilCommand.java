package edu.wpi.first.wpilibj.experimental.command;


import java.util.function.BooleanSupplier;

import edu.wpi.first.wpilibj.Timer;

/**
 * A command that does nothing but ends after a specified match time or condition.  Useful for
 * CommandGroups.
 */
public class WaitUntilCommand extends SendableCommandBase {

  private final BooleanSupplier m_condition;

  /**
   * Creates a new WaitUntilCommand that ends after a given condition becomes true.
   *
   * @param condition the condition to determine when to end
   */
  public WaitUntilCommand(BooleanSupplier condition) {
    m_condition = condition;
  }

  /**
   * Creates a new WaitUntilCommand that ends after a given match time.
   *
   * @param time the match time after which to end, in seconds
   */
  public WaitUntilCommand(double time) {
    this(() -> Timer.getMatchTime() - time > 0);
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
