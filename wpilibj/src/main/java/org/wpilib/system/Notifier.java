// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import static org.wpilib.units.Units.Seconds;
import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.ReentrantLock;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.hardware.hal.NotifierJNI;
import org.wpilib.units.measure.Frequency;
import org.wpilib.units.measure.Time;
import org.wpilib.util.WPIUtilJNI;

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

  @Override
  public void close() {
    int handle = m_notifier.getAndSet(0);
    if (handle == 0) {
      return;
    }
    NotifierJNI.destroyNotifier(handle);
    // Join the thread to ensure the callback has exited.
    if (m_thread.isAlive()) {
      try {
        m_thread.interrupt();
        m_thread.join();
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
    }
    m_thread = null;
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
    m_notifier.set(NotifierJNI.createNotifier());

    m_thread =
        new Thread(
            () -> {
              while (!Thread.interrupted()) {
                int notifier = m_notifier.get();
                if (notifier == 0) {
                  break;
                }
                try {
                  WPIUtilJNI.waitForObject(notifier);
                } catch (InterruptedException ex) {
                  Thread.currentThread().interrupt();
                  break;
                }

                Runnable threadHandler;
                m_processLock.lock();
                try {
                  threadHandler = m_callback;
                } finally {
                  m_processLock.unlock();
                }

                // Call callback
                if (threadHandler != null) {
                  threadHandler.run();
                }

                // Acknowledge the alarm
                NotifierJNI.acknowledgeNotifierAlarm(notifier, false, 0, 0, false);
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
   * @param delay Time in seconds to wait before the callback is called.
   */
  public void startSingle(double delay) {
    NotifierJNI.setNotifierAlarm(m_notifier.get(), (long) (delay * 1e6), 0, false);
  }

  /**
   * Run the callback once after the given delay.
   *
   * @param delay Time to wait before the callback is called.
   */
  public void startSingle(Time delay) {
    startSingle(delay.in(Seconds));
  }

  /**
   * Run the callback periodically with the given period.
   *
   * <p>The user-provided callback should be written so that it completes before the next time it's
   * scheduled to run.
   *
   * @param period Period in seconds after which to call the callback starting one period after the
   *     call to this method.
   */
  public void startPeriodic(double period) {
    long periodMicroS = (long) (period * 1e6);
    NotifierJNI.setNotifierAlarm(m_notifier.get(), periodMicroS, periodMicroS, false);
  }

  /**
   * Run the callback periodically with the given period.
   *
   * <p>The user-provided callback should be written so that it completes before the next time it's
   * scheduled to run.
   *
   * @param period Period after which to call the callback starting one period after the call to
   *     this method.
   */
  public void startPeriodic(Time period) {
    startPeriodic(period.in(Seconds));
  }

  /**
   * Run the callback periodically with the given frequency.
   *
   * <p>The user-provided callback should be written so that it completes before the next time it's
   * scheduled to run.
   *
   * @param frequency Frequency at which to call the callback, starting one period after the call to
   *     this method.
   */
  public void startPeriodic(Frequency frequency) {
    startPeriodic(frequency.asPeriod());
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
    NotifierJNI.cancelNotifierAlarm(m_notifier.get());
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
