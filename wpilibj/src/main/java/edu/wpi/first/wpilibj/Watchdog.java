/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.io.Closeable;
import java.util.HashMap;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

/**
 * A class that's a wrapper around a watchdog timer.
 *
 * <p>When the timer expires, a message is printed to the console and an optional user-provided
 * callback is invoked.
 *
 * <p>The watchdog is initialized disabled, so the user needs to call enable() before use.
 */
@SuppressWarnings("PMD.TooManyMethods")
public class Watchdog implements Closeable, Comparable<Watchdog> {
  // Used for timeout print rate-limiting
  private static final long kMinPrintPeriodMicroS = 1000000; // us

  private long m_startTimeMicroS; // us
  private long m_timeoutMicroS; // us
  private long m_expirationTimeMicroS; // us
  private final Runnable m_callback;
  private long m_lastTimeoutPrintTimeMicroS; // us
  private long m_lastEpochsPrintTimeMicroS; // us

  @SuppressWarnings("PMD.UseConcurrentHashMap")
  private final Map<String, Long> m_epochs = new HashMap<>();
  boolean m_isExpired;

  boolean m_suppressTimeoutMessage;

  static {
    startDaemonThread(Watchdog::schedulerFunc);
  }

  private static final PriorityQueue<Watchdog> m_watchdogs = new PriorityQueue<>();
  private static ReentrantLock m_queueMutex = new ReentrantLock();
  private static Condition m_schedulerWaiter = m_queueMutex.newCondition();

  /**
   * Watchdog constructor.
   *
   * @param timeoutSeconds The watchdog's timeout in seconds with microsecond resolution.
   * @param callback       This function is called when the timeout expires.
   */
  public Watchdog(double timeoutSeconds, Runnable callback) {
    m_timeoutMicroS = (long) (timeoutSeconds * 1.0e6);
    m_callback = callback;
  }

  @Override
  public void close() {
    disable();
  }

  @Override
  public int compareTo(Watchdog rhs) {
    // Elements with sooner expiration times are sorted as lesser. The head of
    // Java's PriorityQueue is the least element.
    return Long.compare(m_expirationTimeMicroS, rhs.m_expirationTimeMicroS);
  }

  /**
   * Returns the time in seconds since the watchdog was last fed.
   */
  public double getTimeSeconds() {
    return (RobotController.getFPGATimeMicroSeconds() - m_startTimeMicroS) / 1.0e6;
  }

  /**
   * Sets the watchdog's timeout.
   *
   * @param timeoutSeconds The watchdog's timeout in seconds with microsecond
   *                       resolution.
   */
  public void setTimeout(double timeoutSeconds) {
    m_startTimeMicroS = RobotController.getFPGATimeMicroSeconds();
    m_epochs.clear();

    m_queueMutex.lock();
    try {
      m_timeoutMicroS = (long) (timeoutSeconds * 1.0e6);
      m_isExpired = false;

      m_watchdogs.remove(this);
      m_expirationTimeMicroS = m_startTimeMicroS + m_timeoutMicroS;
      m_watchdogs.add(this);
      m_schedulerWaiter.signalAll();
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Returns the watchdog's timeout in seconds.
   */
  public double getTimeoutSeconds() {
    m_queueMutex.lock();
    try {
      return m_timeoutMicroS / 1.0e6;
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Returns the watchdog's timeout in seconds.
   *
   * @deprecated Use {@link getTimeoutSeconds} instead.
   */
  @Deprecated(since = "2020")
  public double getTimeout() {
    return getTimeoutSeconds();
  }

  /**
   * Returns true if the watchdog timer has expired.
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
   * <p>Epochs are a way to partition the time elapsed so that when overruns occur, one can
   * determine which parts of an operation consumed the most time.
   *
   * @param epochName The name to associate with the epoch.
   */
  public void addEpoch(String epochName) {
    long currentTimeMicroS = RobotController.getFPGATimeMicroSeconds();
    m_epochs.put(epochName, currentTimeMicroS - m_startTimeMicroS);
    m_startTimeMicroS = currentTimeMicroS;
  }

  /**
   * Prints list of epochs added so far and their times.
   */
  public void printEpochs() {
    long nowMicroS = RobotController.getFPGATimeMicroSeconds();
    if (nowMicroS  - m_lastEpochsPrintTimeMicroS > kMinPrintPeriodMicroS) {
      m_lastEpochsPrintTimeMicroS = nowMicroS;
      m_epochs.forEach((key, value) -> System.out.format("\t%s: %.6fs\n", key, value / 1.0e6));
    }
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
    m_startTimeMicroS = RobotController.getFPGATimeMicroSeconds();
    m_epochs.clear();

    m_queueMutex.lock();
    try {
      m_isExpired = false;

      m_watchdogs.remove(this);
      m_expirationTimeMicroS = m_startTimeMicroS + m_timeoutMicroS;
      m_watchdogs.add(this);
      m_schedulerWaiter.signalAll();
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Disables the watchdog timer.
   */
  public void disable() {
    m_queueMutex.lock();
    try {
      m_watchdogs.remove(this);
      m_schedulerWaiter.signalAll();
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

  private static Thread startDaemonThread(Runnable target) {
    Thread inst = new Thread(target);
    inst.setDaemon(true);
    inst.start();
    return inst;
  }


  @SuppressWarnings("PMD.AvoidDeeplyNestedIfStmts")
  private static void schedulerFunc() {
    m_queueMutex.lock();

    try {
      while (!Thread.currentThread().isInterrupted()) {
        if (m_watchdogs.size() > 0) {
          boolean timedOut = !awaitUntil(m_schedulerWaiter,
              m_watchdogs.peek().m_expirationTimeMicroS);
          if (timedOut) {
            if (m_watchdogs.size() == 0 || m_watchdogs.peek().m_expirationTimeMicroS
                > RobotController.getFPGATimeMicroSeconds()) {
              continue;
            }

            // If the condition variable timed out, that means a Watchdog timeout
            // has occurred, so call its timeout function.
            Watchdog watchdog = m_watchdogs.poll();

            long nowMicroS = RobotController.getFPGATimeMicroSeconds();
            if (nowMicroS - watchdog.m_lastTimeoutPrintTimeMicroS > kMinPrintPeriodMicroS) {
              watchdog.m_lastTimeoutPrintTimeMicroS = nowMicroS;
              if (!watchdog.m_suppressTimeoutMessage) {
                System.out.format("Watchdog not fed within %.6fs\n",
                    watchdog.m_timeoutMicroS / 1.0e6);
              }
            }

            // Set expiration flag before calling the callback so any
            // manipulation of the flag in the callback (e.g., calling
            // Disable()) isn't clobbered.
            watchdog.m_isExpired = true;

            m_queueMutex.unlock();
            watchdog.m_callback.run();
            m_queueMutex.lock();
          }
          // Otherwise, a Watchdog removed itself from the queue (it notifies
          // the scheduler of this) or a spurious wakeup occurred, so just
          // rewait with the soonest watchdog timeout.
        } else {
          while (m_watchdogs.size() == 0) {
            m_schedulerWaiter.awaitUninterruptibly();
          }
        }
      }
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Wrapper emulating functionality of C++'s std::condition_variable::wait_until().
   *
   * @param cond             The condition variable on which to wait.
   * @param timeMicroSeconds The time at which to stop waiting in microseconds.
   * @return False if the deadline has elapsed upon return, else true.
   */
  private static boolean awaitUntil(Condition cond, long timeMicroSeconds) {
    long delta = timeMicroSeconds - RobotController.getFPGATimeMicroSeconds();
    try {
      return cond.await(delta, TimeUnit.MICROSECONDS);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      ex.printStackTrace();
    }

    return true;
  }
}
