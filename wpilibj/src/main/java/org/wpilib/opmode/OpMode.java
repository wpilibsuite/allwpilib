// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.opmode;

import org.wpilib.framework.OpModeRobot;
import org.wpilib.system.RobotController;
import org.wpilib.units.measure.Time;
import org.wpilib.util.CallbackQueue;

import static org.wpilib.units.Units.Seconds;

/**
 * Top-level superclass for opmode classes.
 *
 * <p>Each opmode instance has a single lifecycle: it is constructed when selected, can run while
 * selected/enabled, and is closed when deselected or after an enabled run ends.
 */
public abstract class OpMode {
  private final CallbackQueue m_callbackQueue;
  private final long m_startTimeUs = RobotController.getFPGATime();

  protected OpMode() {
    m_callbackQueue = new CallbackQueue();
    m_callbackQueue.add(this::periodic, m_startTimeUs, Seconds.of(0.02));
  }

  /**
   * This function is called periodically while the opmode is selected and the robot is disabled.
   * Code that should only run once when the opmode is selected should go in the opmode constructor.
   */
  public void disabledPeriodic() {}

  /** Called once when this opmode transitions to enabled. */
  public void start() {}

  /**
   * This function is called periodically while the opmode is enabled at the rate returned by {@link
   * OpModeRobot#getPeriod()}.
   */
  public void periodic() {}

  /**
   * This function is called asynchronously when the robot disables or switches opmodes while this
   * opmode is enabled. Implementations should stop blocking work promptly.
   */
  public void stop() {}

  /**
   * This function is called when the opmode is no longer selected on the DS or after an enabled run
   * ends. The object will not be reused after this is called.
   */
  public void close() {}

  public CallbackQueue getCallbackQueue() {
    return m_callbackQueue;
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback in seconds.
   */
  public final void addPeriodic(Runnable callback, double period) {
    addPeriodic(callback, Seconds.of(period), Seconds.of(period));
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
    addPeriodic(callback, Seconds.of(period), Seconds.of(offset));
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   */
  public final void addPeriodic(Runnable callback, Time period) {
    m_callbackQueue.add(callback, m_startTimeUs, period, Seconds.of(0.0));
  }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   * @param offset The offset from the common starting time. This is useful for scheduling a
   *     callback in a different timeslot relative to TimedRobot.
   */
  public final void addPeriodic(Runnable callback, Time period, Time offset) {
    m_callbackQueue.add(callback, m_startTimeUs, period, offset);
  }
}
