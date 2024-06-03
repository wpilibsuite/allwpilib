// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.NotifierJNI;
import java.io.Closeable;
import java.util.PriorityQueue;
import java.util.concurrent.locks.ReentrantLock;

/**
 * A class that's a wrapper around a watchdog timer.
 *
 * <p>When the timer expires, a message is printed to the console and an optional user-provided
 * callback is invoked.
 *
 * <p>The watchdog is initialized disabled, so the user needs to call enable() before use.
 */
public class Watchdog implements Closeable, Comparable<Watchdog> {
  // Used for timeout print rate-limiting
  private static final long kMinPrintPeriodMicroS = (long) 1e6;

  private double m_startTimeSeconds;
  private double m_timeoutSeconds;
  private double m_expirationTimeSeconds;
  private final Runnable m_callback;
  private double m_lastTimeoutPrintSeconds;

  boolean m_isExpired;

  boolean m_suppressTimeoutMessage;

  private final Tracer m_tracer;

  private static final PriorityQueue<Watchdog> m_watchdogs = new PriorityQueue<>();
  private static ReentrantLock m_queueMutex = new ReentrantLock();
  private static int m_notifier;

  static {
    m_notifier = NotifierJNI.initializeNotifier();
    NotifierJNI.setNotifierName(m_notifier, "Watchdog");
    startDaemonThread(Watchdog::schedulerFunc);
  }

  /**
   * Watchdog constructor.
   *
   * @param timeoutSeconds The watchdog's timeout in seconds with microsecond resolution.
   * @param callback This function is called when the timeout expires.
   */
  public Watchdog(double timeoutSeconds, Runnable callback) {
    m_timeoutSeconds = timeoutSeconds;
    m_callback = callback;
    m_tracer = new Tracer();
  }

  @Override
  public void close() {
    disable();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof Watchdog watchdog
        && Double.compare(m_expirationTimeSeconds, watchdog.m_expirationTimeSeconds) == 0;
  }

  @Override
  public int hashCode() {
    return Double.hashCode(m_expirationTimeSeconds);
  }

  @Override
  public int compareTo(Watchdog rhs) {
    // Elements with sooner expiration times are sorted as lesser. The head of
    // Java's PriorityQueue is the least element.
    return Double.compare(m_expirationTimeSeconds, rhs.m_expirationTimeSeconds);
  }

  /**
   * Returns the time in seconds since the watchdog was last fed.
   *
   * @return The time in seconds since the watchdog was last fed.
   */
  public double getTime() {
    return Timer.getFPGATimestamp() - m_startTimeSeconds;
  }

  /**
   * Sets the watchdog's timeout.
   *
   * @param timeoutSeconds The watchdog's timeout in seconds with microsecond resolution.
   */
  public void setTimeout(double timeoutSeconds) {
    m_startTimeSeconds = Timer.getFPGATimestamp();
    m_tracer.clearEpochs();

    m_queueMutex.lock();
    try {
      m_timeoutSeconds = timeoutSeconds;
      m_isExpired = false;

      m_watchdogs.remove(this);
      m_expirationTimeSeconds = m_startTimeSeconds + m_timeoutSeconds;
      m_watchdogs.add(this);
      updateAlarm();
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Returns the watchdog's timeout in seconds.
   *
   * @return The watchdog's timeout in seconds.
   */
  public double getTimeout() {
    m_queueMutex.lock();
    try {
      return m_timeoutSeconds;
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Returns true if the watchdog timer has expired.
   *
   * @return True if the watchdog timer has expired.
   */
  public boolean isExpired() {
    m_queueMutex.lock();
    try {
      return m_isExpired;
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Adds time since last epoch to the list printed by printEpochs().
   *
   * @see Tracer#addEpoch(String)
   * @param epochName The name to associate with the epoch.
   */
  public void addEpoch(String epochName) {
    m_tracer.addEpoch(epochName);
  }

  /**
   * Prints list of epochs added so far and their times.
   *
   * @see Tracer#printEpochs()
   */
  public void printEpochs() {
    m_tracer.printEpochs();
  }

  /**
   * Resets the watchdog timer.
   *
   * <p>This also enables the timer if it was previously disabled.
   */
  public void reset() {
    enable();
  }

  /** Enables the watchdog timer. */
  public void enable() {
    m_startTimeSeconds = Timer.getFPGATimestamp();
    m_tracer.clearEpochs();

    m_queueMutex.lock();
    try {
      m_isExpired = false;

      m_watchdogs.remove(this);
      m_expirationTimeSeconds = m_startTimeSeconds + m_timeoutSeconds;
      m_watchdogs.add(this);
      updateAlarm();
    } finally {
      m_queueMutex.unlock();
    }
  }

  /** Disables the watchdog timer. */
  public void disable() {
    m_queueMutex.lock();
    try {
      m_watchdogs.remove(this);
      updateAlarm();
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Enable or disable suppression of the generic timeout message.
   *
   * <p>This may be desirable if the user-provided callback already prints a more specific message.
   *
   * @param suppress Whether to suppress generic timeout message.
   */
  public void suppressTimeoutMessage(boolean suppress) {
    m_suppressTimeoutMessage = suppress;
  }

  @SuppressWarnings("resource")
  private static void updateAlarm() {
    if (m_watchdogs.isEmpty()) {
      NotifierJNI.cancelNotifierAlarm(m_notifier);
    } else {
      NotifierJNI.updateNotifierAlarm(
          m_notifier, (long) (m_watchdogs.peek().m_expirationTimeSeconds * 1e6));
    }
  }

  private static Thread startDaemonThread(Runnable target) {
    Thread inst = new Thread(target);
    inst.setDaemon(true);
    inst.start();
    return inst;
  }

  private static void schedulerFunc() {
    while (!Thread.currentThread().isInterrupted()) {
      long curTime = NotifierJNI.waitForNotifierAlarm(m_notifier);
      if (curTime == 0) {
        break;
      }

      m_queueMutex.lock();
      try {
        if (m_watchdogs.isEmpty()) {
          continue;
        }

        // If the condition variable timed out, that means a Watchdog timeout
        // has occurred, so call its timeout function.
        Watchdog watchdog = m_watchdogs.poll();

        double now = curTime * 1e-6;
        if (now - watchdog.m_lastTimeoutPrintSeconds > kMinPrintPeriodMicroS) {
          watchdog.m_lastTimeoutPrintSeconds = now;
          if (!watchdog.m_suppressTimeoutMessage) {
            DriverStation.reportWarning(
                String.format("Watchdog not fed within %.6fs\n", watchdog.m_timeoutSeconds), false);
          }
        }

        // Set expiration flag before calling the callback so any
        // manipulation of the flag in the callback (e.g., calling
        // Disable()) isn't clobbered.
        watchdog.m_isExpired = true;

        m_queueMutex.unlock();
        watchdog.m_callback.run();
        m_queueMutex.lock();

        updateAlarm();
      } finally {
        m_queueMutex.unlock();
      }
    }
  }
}
