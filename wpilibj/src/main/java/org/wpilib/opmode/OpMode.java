// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.opmode;

/**
 * Top-level interface for opmode classes.
 *
 * <p>Each opmode instance has a single lifecycle: it is constructed when selected, can run while
 * selected/enabled, and is closed when deselected or after an enabled run ends.
 */
public interface OpMode {
  /**
   * This function is called periodically while the opmode is selected and the robot is disabled.
   * Code that should only run once when the opmode is selected should go in the opmode constructor.
   */
  default void disabledPeriodic() {}

  /** Called once when this opmode transitions to enabled. */
  default void opModeStart() {}

  /**
   * This function is called periodically while the opmode is enabled at the rate returned by {@link
   * #getPeriod()}.
   */
  default void opModePeriodic() {}

  /**
   * This function is called asynchronously when the robot disables or switches opmodes while this
   * opmode is enabled. Implementations should stop blocking work promptly.
   */
  default void opModeStop() {}

  /**
   * This function is called when the opmode is no longer selected on the DS or after an enabled run
   * ends. The object will not be reused after this is called.
   */
  default void opModeClose() {}

  /**
   * Returns the periodic interval in seconds for {@link #opModePeriodic()} while enabled.
   *
   * @return periodic interval in seconds (default 20 ms)
   */
  default double getPeriod() {
    return 0.02;
  }
}
