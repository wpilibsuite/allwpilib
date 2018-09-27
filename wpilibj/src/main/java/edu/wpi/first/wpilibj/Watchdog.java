/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.HashMap;
import java.util.Map;

/**
 * A class that's a wrapper around a watchdog timer.
 *
 * <p>When the timer expires, a message is printed to the console and an optional user-provided
 * callback is invoked.
 *
 * <p>The watchdog is initialized disabled, so the user needs to call enable() before use.
 */
public class Watchdog {
  private double m_timeout;
  private Runnable m_callback;
  private Notifier m_notifier;

  private double m_startTime;
  @SuppressWarnings("PMD.UseConcurrentHashMap")
  private final Map<String, Double> m_epochs = new HashMap<>();
  boolean m_isExpired;

  /**
   * Watchdog constructor.
   *
   * @param timeout The watchdog's timeout in seconds.
   */
  public Watchdog(double timeout) {
    this(timeout, () -> {
    });
  }

  /**
   * Watchdog constructor.
   *
   * @param timeout  The watchdog's timeout in seconds.
   * @param callback This function is called when the timeout expires.
   */
  public Watchdog(double timeout, Runnable callback) {
    m_timeout = timeout;
    m_callback = callback;
    m_notifier = new Notifier(this::timeoutFunc);
    enable();
  }

  /**
   * Get the time in seconds since the watchdog was last fed.
   */
  public double getTime() {
    return Timer.getFPGATimestamp() - m_startTime;
  }

  /**
   * Returns true if the watchdog timer has expired.
   */
  public boolean isExpired() {
    return m_isExpired;
  }

  /**
   * Adds time since last epoch to the list printed by printEpochs().
   *
   * <p>Epochs are a way to partition the time elapsed so that when overruns occur, one can
   * determine which parts of an operation consumed the most time.
   *
   * @param epochName The name to associate with the epoch.
   */
  public void addEpoch(String epochName) {
    double currentTime = Timer.getFPGATimestamp();
    m_epochs.put(epochName, currentTime - m_startTime);
    m_startTime = currentTime;
  }

  /**
   * Prints list of epochs added so far and their times.
   */
  public void printEpochs() {
    m_epochs.forEach((key, value) -> {
      System.out.println("\t" + key + ": " + value + "s");
    });
  }

  /**
   * Resets the watchdog timer.
   *
   * <p>This also enables the timer if it was previously disabled.
   */
  public void reset() {
    enable();
  }

  /**
   * Enables the watchdog timer.
   */
  public void enable() {
    m_startTime = Timer.getFPGATimestamp();
    m_isExpired = false;
    m_epochs.clear();
    m_notifier.startPeriodic(m_timeout);
  }

  /**
   * Disable the watchdog.
   */
  public void disable() {
    m_notifier.stop();
  }

  private void timeoutFunc() {
    if (!m_isExpired) {
      System.out.println("Watchdog not fed after " + m_timeout + "s");
      m_callback.run();
      m_isExpired = true;
      disable();
    }
  }
}
