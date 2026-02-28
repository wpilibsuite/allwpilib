// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.function.BooleanSupplier;
import org.wpilib.system.Timer;

/**
 * A command that does nothing but ends after a specified match time or condition. Useful for
 * CommandGroups.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class WaitUntilCommand extends Command {
  private final BooleanSupplier m_condition;

  /**
   * Creates a new WaitUntilCommand that ends after a given condition becomes true.
   *
   * @param condition the condition to determine when to end
   */
  public WaitUntilCommand(BooleanSupplier condition) {
    m_condition = requireNonNullParam(condition, "condition", "WaitUntilCommand");
  }

  /**
   * Creates a new WaitUntilCommand that ends after a given match time.
   *
   * <p>NOTE: The match timer used for this command is UNOFFICIAL. Using this command does NOT
   * guarantee that the time at which the action is performed will be judged to be legal by the
   * referees. When in doubt, add a safety factor or time the action manually.
   *
   * <p>The match time counts down when connected to FMS or the DS is in practice mode for the
   * current mode. When the DS is not connected to FMS or in practice mode, the command will not
   * wait.
   *
   * @param time the match time after which to end, in seconds
   * @see org.wpilib.driverstation.DriverStation#getMatchTime()
   */
  public WaitUntilCommand(double time) {
    this(() -> Timer.getMatchTime() < time);
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
