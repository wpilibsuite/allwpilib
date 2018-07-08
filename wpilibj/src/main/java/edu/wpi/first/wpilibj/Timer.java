/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

public class Timer {
  /**
   * Return the system clock time in seconds. Return the time from the FPGA hardware clock in
   * seconds since the FPGA started.
   *
   * @return Robot running time in seconds.
   */
  @SuppressWarnings("AbbreviationAsWordInName")
  public static double getFPGATimestamp() {
    return RobotController.getFPGATime() / 1000000.0;
  }

  /**
   * Return the approximate match time. The FMS does not send an official match time to the robots,
   * but does send an approximate match time. The value will count down the time remaining in the
   * current period (auto or teleop). Warning: This is not an official time (so it cannot be used to
   * dispute ref calls or guarantee that a function will trigger before the match ends) The
   * Practice Match function of the DS approximates the behaviour seen on the field.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  public static double getMatchTime() {
    return DriverStation.getInstance().getMatchTime();
  }

  /**
   * Pause the thread for a specified time. Pause the execution of the thread for a specified period
   * of time given in seconds. Motors will continue to run at their last assigned values, and
   * sensors will continue to update. Only the task containing the wait will pause until the wait
   * time is expired.
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

  private double m_startTime;
  private double m_accumulatedTime;
  private boolean m_running;

  @SuppressWarnings("JavadocMethod")
  public Timer() {
    reset();
  }

  private double getMsClock() {
    return RobotController.getFPGATime() / 1000.0;
  }

  /**
   * Get the current time from the timer. If the clock is running it is derived from the current
   * system clock the start time stored in the timer class. If the clock is not running, then return
   * the time when it was last stopped.
   *
   * @return Current time value for this timer in seconds
   */
  public synchronized double get() {
    if (m_running) {
      return ((getMsClock() - m_startTime) + m_accumulatedTime) / 1000.0;
    } else {
      return m_accumulatedTime;
    }
  }

  /**
   * Reset the timer by setting the time to 0. Make the timer startTime the current time so new
   * requests will be relative now
   */
  public synchronized void reset() {
    m_accumulatedTime = 0;
    m_startTime = getMsClock();
  }

  /**
   * Start the timer running. Just set the running flag to true indicating that all time requests
   * should be relative to the system clock.
   */
  public synchronized void start() {
    m_startTime = getMsClock();
    m_running = true;
  }

  /**
   * Stop the timer. This computes the time as of now and clears the running flag, causing all
   * subsequent time requests to be read from the accumulated time rather than looking at the system
   * clock.
   */
  public synchronized void stop() {
    final double temp = get();
    m_accumulatedTime = temp;
    m_running = false;
  }

  /**
   * Check if the period specified has passed and if it has, advance the start time by that period.
   * This is useful to decide if it's time to do periodic work without drifting later by the time it
   * took to get around to checking.
   *
   * @param period The period to check for (in seconds).
   * @return If the period has passed.
   */
  public synchronized boolean hasPeriodPassed(double period) {
    if (get() > period) {
      // Advance the start time by the period.
      // Don't set it to the current time... we want to avoid drift.
      m_startTime += period * 1000;
      return true;
    }
    return false;
  }
}
