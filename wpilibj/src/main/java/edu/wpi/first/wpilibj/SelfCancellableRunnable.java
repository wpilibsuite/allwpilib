// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.units.measure.Time;
import edu.wpi.first.wpilibj.TimedRobot.Cancellable;

public abstract class SelfCancellableRunnable implements Runnable {

  private Cancellable m_cancelThisCallback;

  /**
   * Schedule this {@link SelfCancellableRunnable} to run every {@code periodSeconds} seconds on the
   * specified {@code robot}
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param robot The {@link TimedRobot} to run this {@link SelfCancellableRunnable} {@code robot}
   *     must <em>not</em> be {@code null}.
   * @param periodSeconds The period at which to run the callback in seconds.
   * @return a {@link TimedRobot.Cancellable} that allows the user to cancel periodic invocation.
   * @see TimedRobot#addPeriodic(Runnable, double)
   */
  public Cancellable schedulePeriodic(TimedRobot robot, double periodSeconds) {
    return m_cancelThisCallback = robot.addPeriodic(this, periodSeconds);
  }

  /**
   * Schedule this {@link SelfCancellableRunnable} to run every {@code periodSeconds} on {@code
   * robot}. Offset invocation by {@code offsetSeconds} seconds.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param robot The {@link TimedRobot} to run this {@link SelfCancellableRunnable} {@code robot}
   *     must <em>not</em> be {@code null}.
   * @param periodSeconds The period at which to run the callback in seconds.
   * @param offsetSeconds The offset from the common starting time in seconds. This is useful for
   *     scheduling a callback in a different timeslot relative to TimedRobot.
   * @return a {@link TimedRobot.Cancellable} that allows the user to cancel periodic invocation.
   * @see TimedRobot#addPeriodic(Runnable, double, double)
   */
  public final Cancellable schedulePeriodic(
      TimedRobot robot, double periodSeconds, double offsetSeconds) {
    return m_cancelThisCallback = robot.addPeriodic(this, periodSeconds, offsetSeconds);
  }

  /**
   * Schedule this {@link SelfCancellableRunnable} to run periodically on the specified {@link
   * TimedRobot} at the specified {@code period}.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param robot The {@link TimedRobot} to run this {@link SelfCancellableRunnable} {@code robot}
   *     must <em>not</em> be {@code null}.
   * @param period The period at which to run the callback.
   * @return A {@link Cancellable} that allows the user to cancel periodic invocation.
   * @see TimedRobot#addPeriodic(Runnable, Time)
   */
  public final Cancellable schedulePeriodic(TimedRobot robot, Time period) {
    return m_cancelThisCallback = robot.addPeriodic(this, period);
  }

  /**
   * Schedule this {@link SelfCancellableRunnable} to run periodically at the specified {@code
   * period} on the specified {@code robot}. Offset invocation by the specified {@code offset}.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param robot The {@link TimedRobot} to run this {@link SelfCancellableRunnable} {@code robot}
   *     must <em>not</em> be {@code null}.
   * @param period The period at which to run the callback.
   * @param offset The offset from the common starting time in seconds. This is useful for
   *     scheduling a callback in a different timeslot relative to TimedRobot.
   * @return A {@link Cancellable} that allows the user to cancel periodic invocation.
   * @see TimedRobot#addPeriodic(Runnable, Time, Time)
   */
  public final Cancellable schedulePeriodic(TimedRobot robot, Time period, Time offset) {
    return m_cancelThisCallback = robot.addPeriodic(this, period, offset);
  }

  /** Cancel periodic invocation of this {@link SelfCancellableRunnable}. */
  protected void cancel() {
    m_cancelThisCallback.cancel();
  }
}
