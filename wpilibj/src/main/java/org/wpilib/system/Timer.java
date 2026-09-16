// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import static org.wpilib.units.Units.Seconds;

import org.wpilib.driverstation.MatchState;
import org.wpilib.units.measure.Time;

/**
 * A timer class.
 *
 * <p>Note that if the user calls SimHooks.restartTiming(), they should also reset the timer so
 * get() won't return a negative duration.
 */
public class Timer {
  /**
   * Return the clock time in seconds. By default, the time is the time returned by
   * getMonotonicTimestamp(). However, the return value of this method may be modified to use any
   * time base, including non-monotonic time bases.
   *
   * @return Robot running time in seconds.
   */
  public static double getTimestamp() {
    return RobotController.getTime() / 1_000_000_000.0;
  }

  /**
   * Return the monotonic clock time in seconds.
   *
   * @return Monotonic time in seconds.
   */
  public static double getMonotonicTimestamp() {
    return RobotController.getMonotonicTime() / 1_000_000_000.0;
  }

  /**
   * Return the approximate match time. The FMS does not send an official match time to the robots,
   * but does send an approximate match time. The value will count down the time remaining in the
   * current period (auto or teleop). Warning: This is not an official time (so it cannot be used to
   * dispute ref calls or guarantee that a function will trigger before the match ends).
   *
   * <p>When connected to the real field, this number only changes in full integer increments, and
   * always counts down.
   *
   * <p>When the DS is in practice mode, this number is a floating point number, and counts down.
   *
   * <p>When the DS is in teleop or autonomous mode, this number returns -1.0.
   *
   * <p>Simulation matches DS behavior without an FMS connected.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  public static double getMatchTime() {
    return MatchState.getMatchTime();
  }

  /**
   * Pause the execution of the thread for a specified period of time. Motors will continue to run
   * at their last assigned values, and sensors will continue to update. Only the task containing
   * the wait will pause until the wait time is expired.
   *
   * @param period Length of time to pause
   */
  public static void delay(final Time period) {
    delay(period.in(Seconds));
  }

  /**
   * Pause the execution of the thread for a specified period of time given in seconds. Motors will
   * continue to run at their last assigned values, and sensors will continue to update. Only the
   * task containing the wait will pause until the wait time is expired.
   *
   * @param seconds Length of time to pause
   */
  public static void delay(final double seconds) {
    try {
      Thread.sleep((long) (seconds * 1e3));
    } catch (final InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
  }

  private long m_startTimeNs;
  private double m_startTimeRemainderNs;
  private double m_accumulatedTimeNs;
  private boolean m_running;

  /**
   * Creates a new timer. The timer is initially not running and must be started with {@link
   * #start()} to start measuring time. Consider using {@link #createStarted()} instead if the timer
   * will be used immediately after creation.
   */
  public Timer() {
    reset();
  }

  /**
   * Creates a new timer that begins started.
   *
   * <p>This is equivalent to
   *
   * {@snippet lang="java":
   *  Timer timer = new Timer();
   *  timer.start();
   * }
   *
   * @return A new started timer.
   */
  public static Timer createStarted() {
    var timer = new Timer();
    timer.start();
    return timer;
  }

  private long getNsClock() {
    return RobotController.getTime();
  }

  private double getNanoseconds() {
    if (m_running) {
      return getNsClock() - m_startTimeNs - m_startTimeRemainderNs + m_accumulatedTimeNs;
    } else {
      return m_accumulatedTimeNs;
    }
  }

  /**
   * Get the current time from the timer. If the clock is running it is derived from the current
   * system clock the start time stored in the timer class. If the clock is not running, then return
   * the time when it was last stopped.
   *
   * @return Current time value for this timer in seconds
   */
  public double get() {
    return getNanoseconds() / 1_000_000_000.0;
  }

  /**
   * Reset the timer by setting the time to 0.
   *
   * <p>Make the timer startTime the current time so new requests will be relative now.
   */
  public final void reset() {
    m_accumulatedTimeNs = 0.0;
    m_startTimeNs = getNsClock();
    m_startTimeRemainderNs = 0.0;
  }

  /**
   * Start the timer running. Just set the running flag to true indicating that all time requests
   * should be relative to the system clock. Note that this method is a no-op if the timer is
   * already running.
   */
  public void start() {
    if (!m_running) {
      m_startTimeNs = getNsClock();
      m_startTimeRemainderNs = 0.0;
      m_running = true;
    }
  }

  /**
   * Restart the timer by stopping the timer, if it is not already stopped, resetting the
   * accumulated time, then starting the timer again. If you want an event to periodically reoccur
   * at some time interval from the start time, consider using advanceIfElapsed() instead.
   */
  public void restart() {
    if (m_running) {
      stop();
    }
    reset();
    start();
  }

  /**
   * Stop the timer. This computes the time as of now and clears the running flag, causing all
   * subsequent time requests to be read from the accumulated time rather than looking at the system
   * clock.
   */
  public void stop() {
    if (m_running) {
      m_accumulatedTimeNs = getNanoseconds();
      m_running = false;
    }
  }

  /**
   * Check if the period specified has passed.
   *
   * @param period The period to check.
   * @return Whether the period has passed.
   */
  public boolean hasElapsed(Time period) {
    return hasElapsed(period.in(Seconds));
  }

  /**
   * Check if the period specified has passed.
   *
   * @param seconds The period to check in seconds.
   * @return Whether the period has passed.
   */
  public boolean hasElapsed(double seconds) {
    return getNanoseconds() >= seconds * 1e9;
  }

  /**
   * Check if the period specified has passed and if it has, advance the start time by that period.
   * This is useful to decide if it's time to do periodic work without drifting later by the time it
   * took to get around to checking.
   *
   * @param seconds The period to check.
   * @return Whether the period has passed.
   */
  public boolean advanceIfElapsed(double seconds) {
    double periodNs = seconds * 1e9;

    if (getNanoseconds() >= periodNs) {
      // Advance the start time by the period.
      // Don't set it to the current time... we want to avoid drift.
      double advanceNs = m_startTimeRemainderNs + periodNs;
      long wholeNs = (long) advanceNs;
      m_startTimeNs += wholeNs;
      m_startTimeRemainderNs = advanceNs - wholeNs;
      return true;
    } else {
      return false;
    }
  }

  /**
   * Whether the timer is currently running.
   *
   * @return true if running.
   */
  public boolean isRunning() {
    return m_running;
  }
}
