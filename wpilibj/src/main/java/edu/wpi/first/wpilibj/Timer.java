/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.util.BaseSystemNotInitializedException;

public class Timer {
  private static StaticInterface impl;

  @SuppressWarnings("MethodName")
  public static void SetImplementation(StaticInterface ti) {
    impl = ti;
  }

  /**
   * Return the system clock time in seconds. Return the time from the FPGA hardware clock in
   * seconds since the FPGA started.
   *
   * @return Robot running time in seconds.
   */
  @SuppressWarnings("AbbreviationAsWordInName")
  public static double getFPGATimestamp() {
    if (impl != null) {
      return impl.getFPGATimestamp();
    } else {
      throw new BaseSystemNotInitializedException(StaticInterface.class, Timer.class);
    }
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
    if (impl != null) {
      return impl.getMatchTime();
    } else {
      throw new BaseSystemNotInitializedException(StaticInterface.class, Timer.class);
    }
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
    if (impl != null) {
      impl.delay(seconds);
    } else {
      throw new BaseSystemNotInitializedException(StaticInterface.class, Timer.class);
    }
  }

  public interface StaticInterface {
    @SuppressWarnings("AbbreviationAsWordInName")
    double getFPGATimestamp();

    double getMatchTime();

    void delay(double seconds);

    @SuppressWarnings("JavadocMethod")
    Interface newTimer();
  }

  private final Interface m_timer;

  @SuppressWarnings("JavadocMethod")
  public Timer() {
    if (impl != null) {
      m_timer = impl.newTimer();
    } else {
      throw new BaseSystemNotInitializedException(StaticInterface.class, Timer.class);
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
    return m_timer.get();
  }

  /**
   * Reset the timer by setting the time to 0. Make the timer startTime the current time so new
   * requests will be relative now
   */
  public void reset() {
    m_timer.reset();
  }

  /**
   * Start the timer running. Just set the running flag to true indicating that all time requests
   * should be relative to the system clock.
   */
  public void start() {
    m_timer.start();
  }

  /**
   * Stop the timer. This computes the time as of now and clears the running flag, causing all
   * subsequent time requests to be read from the accumulated time rather than looking at the system
   * clock.
   */
  public void stop() {
    m_timer.stop();
  }

  /**
   * Check if the period specified has passed and if it has, advance the start time by that period.
   * This is useful to decide if it's time to do periodic work without drifting later by the time it
   * took to get around to checking.
   *
   * @param period The period to check for (in seconds).
   * @return If the period has passed.
   */
  public boolean hasPeriodPassed(double period) {
    return m_timer.hasPeriodPassed(period);
  }

  public interface Interface {
    /**
     * Get the current time from the timer. If the clock is running it is derived from the current
     * system clock the start time stored in the timer class. If the clock is not running, then
     * return the time when it was last stopped.
     *
     * @return Current time value for this timer in seconds
     */
    double get();

    /**
     * Reset the timer by setting the time to 0. Make the timer startTime the current time so new
     * requests will be relative now
     */
    void reset();

    /**
     * Start the timer running. Just set the running flag to true indicating that all time requests
     * should be relative to the system clock.
     */
    void start();

    /**
     * Stop the timer. This computes the time as of now and clears the running flag, causing all
     * subsequent time requests to be read from the accumulated time rather than looking at the
     * system clock.
     */
    void stop();


    /**
     * Check if the period specified has passed and if it has, advance the start time by that
     * period. This is useful to decide if it's time to do periodic work without drifting later by
     * the time it took to get around to checking.
     *
     * @param period The period to check for (in seconds).
     * @return If the period has passed.
     */
    boolean hasPeriodPassed(double period);
  }
}
