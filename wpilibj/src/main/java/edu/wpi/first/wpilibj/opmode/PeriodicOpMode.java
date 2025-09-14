// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.opmode;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.NotifierJNI;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.wpilibj.RobotController;
import java.util.PriorityQueue;

public abstract class PeriodicOpMode implements OpMode {
  @SuppressWarnings("MemberName")
  static class Callback implements Comparable<Callback> {
    public Runnable func;
    public long period;
    public long expirationTime;

    /**
     * Construct a callback container.
     *
     * @param func The callback to run.
     * @param startTime The common starting point for all callback scheduling in microseconds.
     * @param period The period at which to run the callback in microseconds.
     * @param offset The offset from the common starting time in microseconds.
     */
    Callback(Runnable func, long startTime, long period, long offset) {
      this.func = func;
      this.period = period;
      this.expirationTime =
          startTime
              + offset
              + this.period
              + (RobotController.getFPGATime() - startTime) / this.period * this.period;
    }

    @Override
    public boolean equals(Object rhs) {
      return rhs instanceof Callback callback && expirationTime == callback.expirationTime;
    }

    @Override
    public int hashCode() {
      return Long.hashCode(expirationTime);
    }

    @Override
    public int compareTo(Callback rhs) {
      // Elements with sooner expiration times are sorted as lesser. The head of
      // Java's PriorityQueue is the least element.
      return Long.compare(expirationTime, rhs.expirationTime);
    }
  }

  /** Default loop period. */
  public static final double kDefaultPeriod = 0.02;

  // The C pointer to the notifier object. We don't use it directly, it is
  // just passed to the JNI bindings.
  private final int m_notifier = NotifierJNI.initializeNotifier();

  private long m_startTimeUs;
  private long m_loopStartTimeUs;

  private final PriorityQueue<Callback> m_callbacks = new PriorityQueue<>();

  // periodic opmodes may specify their period; if unspecified, a default period of 20 ms is used
  protected PeriodicOpMode() {
    this(kDefaultPeriod);
  }

  protected PeriodicOpMode(double period) {
    m_startTimeUs = RobotController.getFPGATime();
    addPeriodic(this::periodic, period);
    NotifierJNI.setNotifierName(m_notifier, "TimedRobot");

    HAL.reportUsage("Framework", "TimedRobot");
  }

  @Override
  public void disabledPeriodic() {
    // this code is called periodically while the opmode is selected on the DS (robot is disabled)
  }

  public void close() {
    // this code is called when the opmode is de-selected on the DS
  }

  public void start() {
    // this code is called when the opmode starts (robot is enabled)
  }

  // this code is called periodically while the opmode is running (robot is enabled)
  public abstract void periodic();

  public void end() {
    // this code is called when the opmode ends (robot is disabled)
  }

  /**
   * Return the system clock time in micrseconds for the start of the current periodic loop. This is
   * in the same time base as Timer.getFPGATimestamp(), but is stable through a loop. It is updated
   * at the beginning of every periodic callback (including the normal periodic loop).
   *
   * @return Robot running time in microseconds, as of the start of the current periodic function.
   */
  public long getLoopStartTime() {
    return m_loopStartTimeUs;
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
    m_callbacks.add(new Callback(callback, m_startTimeUs, (long) (period * 1e6), 0));
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
    m_callbacks.add(
        new Callback(callback, m_startTimeUs, (long) (period * 1e6), (long) (offset * 1e6)));
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
    addPeriodic(callback, period.in(Seconds));
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
    addPeriodic(callback, period.in(Seconds), offset.in(Seconds));
  }

  // implements OpMode interface
  @Override
  public final void opmodeRun(long opModeId) {
    start();
    while (true) {
      // We don't have to check there's an element in the queue first because
      // there's always at least one (the constructor adds one). It's reenqueued
      // at the end of the loop.
      var callback = m_callbacks.poll();

      NotifierJNI.updateNotifierAlarm(m_notifier, callback.expirationTime);

      long currentTime = NotifierJNI.waitForNotifierAlarm(m_notifier);
      if (currentTime == 0) {
        break;
      }

      m_loopStartTimeUs = RobotController.getFPGATime();

      DriverStationJNI.observeUserProgramOpMode(opModeId, true);

      callback.func.run();

      // Increment the expiration time by the number of full periods it's behind
      // plus one to avoid rapid repeat fires from a large loop overrun. We
      // assume currentTime ≥ expirationTime rather than checking for it since
      // the callback wouldn't be running otherwise.
      callback.expirationTime +=
          callback.period
              + (currentTime - callback.expirationTime) / callback.period * callback.period;
      m_callbacks.add(callback);

      // Process all other callbacks that are ready to run
      while (m_callbacks.peek().expirationTime <= currentTime) {
        callback = m_callbacks.poll();

        callback.func.run();

        callback.expirationTime +=
            callback.period
                + (currentTime - callback.expirationTime) / callback.period * callback.period;
        m_callbacks.add(callback);
      }
    }
    end();
  }

  @Override
  public final void opmodeStop() {
    NotifierJNI.stopNotifier(m_notifier);
  }

  @Override
  public final void opmodeClose() {
    NotifierJNI.stopNotifier(m_notifier);
    NotifierJNI.cleanNotifier(m_notifier);
    close();
  }
}
