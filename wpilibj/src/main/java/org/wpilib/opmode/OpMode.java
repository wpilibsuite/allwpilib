// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.opmode;

import java.util.Set;
import org.wpilib.framework.OpModeRobot;
import org.wpilib.internal.PeriodicPriorityQueue;

/**
 * Top-level interface for opmode classes. Users should generally extend one of the abstract
 * implementations of this interface (e.g. {@link PeriodicOpMode}) rather than directly implementing
 * this interface.
 *
 * <p><b>Lifecycle</b>:
 *
 * <ul>
 *   <li>constructed when opmode selected on driver station
 *   <li>disabledPeriodic() called periodically as long as DS is disabled. Note this is not called
 *       on a set time interval (it does not use the same time interval as periodic())
 *   <li>when DS transitions from disabled to enabled, start() is called once
 *   <li>while DS is enabled, periodic() is called periodically at {@link OpModeRobot#getPeriod},
 *       and additional periodic callbacks added via addPeriodic() are called periodically on their
 *       set time intervals
 *   <li>when DS transitions from enabled to disabled, or a different opmode is selected on the
 *       driver station when the DS is enabled, end() is called, followed by close(); the object is
 *       not reused
 *   <li>if a different opmode is selected on the driver station when the DS is disabled, only
 *       close() is called; the object is not reused
 * </ul>
 *
 * <p>All lifecycle callbacks and periodic callbacks run synchronously on the same thread that
 * invokes them. Interactions between opmodes and the robot framework do not require additional
 * synchronization.
 *
 * <p>Additional callbacks can be registered by implementing {@link #getCallbacks()} to return a set
 * of {@link PeriodicPriorityQueue.Callback} objects with custom timing. {@link PeriodicOpMode}
 * provides a convenient implementation of this method and utility methods for adding periodic
 * callbacks.
 */
public interface OpMode extends AutoCloseable {
  /**
   * This function is called periodically while the opmode is selected and the robot is disabled.
   * Code that should only run once when the opmode is selected should go in the opmode constructor.
   */
  default void disabledPeriodic() {}

  /** Called once when this opmode transitions to enabled. */
  default void start() {}

  /**
   * This function is called periodically while the opmode is enabled at the rate returned by {@link
   * OpModeRobot#getPeriod()}.
   */
  default void periodic() {}

  /**
   * This function is called asynchronously when the robot disables or switches opmodes while this
   * opmode is enabled. Implementations should stop blocking work promptly.
   */
  default void end() {}

  /**
   * This function is called when the opmode is no longer selected on the DS or after an enabled run
   * ends. The object will not be reused after this is called.
   */
  @Override
  default void close() {}

  /**
   * Returns a set of custom periodic callbacks to be executed while the opmode is enabled.
   *
   * <p>This method allows opmodes to register arbitrary periodic callbacks with custom execution
   * intervals. The callbacks are executed by the robot framework at their scheduled times, in
   * addition to the primary {@link #periodic()} callback.
   *
   * @return A set of custom callbacks to execute, or an empty set if no custom callbacks are
   *     needed. The default implementation returns an empty set.
   */
  default Set<PeriodicPriorityQueue.Callback> getCallbacks() {
    return Set.of();
  }
}
