// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.opmode;

import java.util.Collections;
import java.util.Set;
import java.util.TreeSet;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.internal.PeriodicPriorityQueue;
import org.wpilib.system.RobotController;

/**
 * An opmode structure for periodic operation. This base class implements a loop that runs one or
 * more functions periodically (on a set time interval aka loop period). The primary periodic
 * callback function is the abstract periodic() function; the time interval for this callback is 20
 * ms by default, but may be changed via passing a different time interval to OpModeRobot's
 * constructor. Additional periodic callbacks with different intervals can be added using the
 * addPeriodic() set of functions.
 *
 * <p>Lifecycle:
 *
 * <ul>
 *   <li>constructed when opmode selected on driver station
 *   <li>disabledPeriodic() called periodically as long as DS is disabled. Note this is not called
 *       on a set time interval (it does not use the same time interval as periodic())
 *   <li>when DS transitions from disabled to enabled, start() is called once
 *   <li>while DS is enabled, periodic() is called periodically on the time interval set by
 *       OpModeRobot's constructor, and additional periodic callbacks added via addPeriodic() are
 *       called periodically on their set time intervals
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
 */
public abstract class PeriodicOpMode implements OpMode {
  private final Set<PeriodicPriorityQueue.Callback> m_callbacks;
  private final long m_startTimeUs = RobotController.getMonotonicTime();

  /** Constructor for PeriodicOpMode. */
  @SuppressWarnings("this-escape")
  protected PeriodicOpMode() {
    m_callbacks = new TreeSet<>();
    HAL.reportUsage("OpMode", "PeriodicOpMode");
  }

  @Override
  public Set<PeriodicPriorityQueue.Callback> getCallbacks() {
    return Collections.unmodifiableSet(m_callbacks);
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on OpModeRobot's Notifier, so OpModeRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback in seconds.
   */
  public final void addPeriodic(Runnable callback, double period) {
    addPeriodic(callback, period, period);
  }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback in seconds.
   * @param offset The offset from the common starting time in seconds. This is useful for
   *     scheduling a callback in a different timeslot relative to TimedRobot.
   */
  public final void addPeriodic(Runnable callback, double period, double offset) {
    m_callbacks.add(new PeriodicPriorityQueue.Callback(callback, m_startTimeUs, period, offset));
  }
}
