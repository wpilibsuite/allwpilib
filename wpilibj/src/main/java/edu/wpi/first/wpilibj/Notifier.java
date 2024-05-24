// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.NotifierJNI;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Notifiers run a user-provided callback function on a separate thread.
 *
 * <p>If startSingle() is used, the callback will run once. If startPeriodic() is used, the callback
 * will run repeatedly with the given period until stop() is called.
 */
public class Notifier implements AutoCloseable {
  // The thread waiting on the HAL alarm.
  private Thread m_thread;

  // The lock held while updating process information.
  private final ReentrantLock m_processLock = new ReentrantLock();

  // HAL handle passed to the JNI bindings (atomic for proper destruction).
  private final AtomicInteger m_notifier = new AtomicInteger();

  // The user-provided callback.
  private Runnable m_callback;

  // The time, in seconds, at which the callback should be called. Has the same
  // zero as RobotController.getFPGATime().
  private double m_expirationTimeSeconds;

  // If periodic, stores the callback period; if single, stores the time until
  // the callback call.
  private double m_periodSeconds;

  // True if the callback is periodic
  private boolean m_periodic;

  @Override
  public void close() {
    int handle = m_notifier.getAndSet(0);
    if (handle == 0) {
      return;
    }
    NotifierJNI.stopNotifier(handle);
    // Join the thread to ensure the callback has exited.
    if (m_thread.isAlive()) {
      try {
        m_thread.interrupt();
        m_thread.join();
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
    }
    NotifierJNI.cleanNotifier(handle);
    m_thread = null;
  }

  /**
   * Update the alarm hardware to reflect the next alarm.
   *
   * @param triggerTimeMicroS the time in microseconds at which the next alarm will be triggered
   */
  private void updateAlarm(long triggerTimeMicroS) {
    int notifier = m_notifier.get();
    if (notifier == 0) {
      return;
    }
    NotifierJNI.updateNotifierAlarm(notifier, triggerTimeMicroS);
  }

  /** Update the alarm hardware to reflect the next alarm. */
  private void updateAlarm() {
    updateAlarm((long) (m_expirationTimeSeconds * 1e6));
  }

  /**
   * Create a Notifier with the given callback.
   *
   * <p>Configure when the callback runs with startSingle() or startPeriodic().
   *
   * @param callback The callback to run.
   */
  public Notifier(Runnable callback) {
    requireNonNullParam(callback, "callback", "Notifier");

    m_callback = callback;
    m_notifier.set(NotifierJNI.initializeNotifier());

    m_thread =
        new Thread(
            () -> {
              while (!Thread.interrupted()) {
                int notifier = m_notifier.get();
                if (notifier == 0) {
                  break;
                }
                long curTime = NotifierJNI.waitForNotifierAlarm(notifier);
                if (curTime == 0) {
                  break;
                }

                Runnable threadHandler;
                m_processLock.lock();
                try {
                  threadHandler = m_callback;
                  if (m_periodic) {
                    m_expirationTimeSeconds += m_periodSeconds;
                    updateAlarm();
                  } else {
                    // Need to update the alarm to cause it to wait again
                    updateAlarm(-1);
                  }
                } finally {
                  m_processLock.unlock();
                }

                // Call callback
                if (threadHandler != null) {
                  threadHandler.run();
                }
              }
            });
    m_thread.setName("Notifier");
    m_thread.setDaemon(true);
    m_thread.setUncaughtExceptionHandler(
        (thread, error) -> {
          Throwable cause = error.getCause();
          if (cause != null) {
            error = cause;
          }
          DriverStation.reportError(
              "Unhandled exception in Notifier thread: " + error, error.getStackTrace());
          DriverStation.reportError(
              "The Runnable for this Notifier (or methods called by it) should have handled "
                  + "the exception above.\n"
                  + "  The above stacktrace can help determine where the error occurred.\n"
                  + "  See https://wpilib.org/stacktrace for more information.",
              false);
        });
    m_thread.start();
  }

  /**
   * Sets the name of the notifier. Used for debugging purposes only.
   *
   * @param name Name
   */
  public void setName(String name) {
    m_thread.setName(name);
    NotifierJNI.setNotifierName(m_notifier.get(), name);
  }

  /**
   * Change the callback function.
   *
   * @param callback The callback function.
   */
  public void setCallback(Runnable callback) {
    m_processLock.lock();
    try {
      m_callback = callback;
    } finally {
      m_processLock.unlock();
    }
  }

  /**
   * Run the callback once after the given delay.
   *
   * @param delaySeconds Time in seconds to wait before the callback is called.
   */
  public void startSingle(double delaySeconds) {
    m_processLock.lock();
    try {
      m_periodic = false;
      m_periodSeconds = delaySeconds;
      m_expirationTimeSeconds = RobotController.getFPGATime() * 1e-6 + delaySeconds;
      updateAlarm();
    } finally {
      m_processLock.unlock();
    }
  }

  /**
   * Run the callback periodically with the given period.
   *
   * <p>The user-provided callback should be written so that it completes before the next time it's
   * scheduled to run.
   *
   * @param periodSeconds Period in seconds after which to to call the callback starting one period
   *     after the call to this method.
   */
  public void startPeriodic(double periodSeconds) {
    m_processLock.lock();
    try {
      m_periodic = true;
      m_periodSeconds = periodSeconds;
      m_expirationTimeSeconds = RobotController.getFPGATime() * 1e-6 + periodSeconds;
      updateAlarm();
    } finally {
      m_processLock.unlock();
    }
  }

  /**
   * Stop further callback invocations.
   *
   * <p>No further periodic callbacks will occur. Single invocations will also be cancelled if they
   * haven't yet occurred.
   *
   * <p>If a callback invocation is in progress, this function will block until the callback is
   * complete.
   */
  public void stop() {
    m_processLock.lock();
    try {
      m_periodic = false;
      NotifierJNI.cancelNotifierAlarm(m_notifier.get());
    } finally {
      m_processLock.unlock();
    }
  }

  /**
   * Sets the HAL notifier thread priority.
   *
   * <p>The HAL notifier thread is responsible for managing the FPGA's notifier interrupt and waking
   * up user's Notifiers when it's their time to run. Giving the HAL notifier thread real-time
   * priority helps ensure the user's real-time Notifiers, if any, are notified to run in a timely
   * manner.
   *
   * @param realTime Set to true to set a real-time priority, false for standard priority.
   * @param priority Priority to set the thread to. For real-time, this is 1-99 with 99 being
   *     highest. For non-real-time, this is forced to 0. See "man 7 sched" for more details.
   * @return True on success.
   */
  public static boolean setHALThreadPriority(boolean realTime, int priority) {
    return NotifierJNI.setHALThreadPriority(realTime, priority);
  }
}
