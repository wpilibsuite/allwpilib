/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.ReentrantLock;

import edu.wpi.first.wpilibj.hal.NotifierJNI;

public class Notifier {
  // The thread waiting on the HAL alarm.
  private final Thread m_thread;
  // The lock for the process information.
  private final ReentrantLock m_processLock = new ReentrantLock();
  // The C pointer to the notifier object. We don't use it directly, it is
  // just passed to the JNI bindings.
  private final AtomicInteger m_notifier = new AtomicInteger();
  // The time, in microseconds, at which the corresponding handler should be
  // called. Has the same zero as Utility.getFPGATime().
  private double m_expirationTime = 0;
  // The handler passed in by the user which should be called at the
  // appropriate interval.
  private Runnable m_handler;
  // Whether we are calling the handler just once or periodically.
  private boolean m_periodic = false;
  // If periodic, the period of the calling; if just once, stores how long it
  // is until we call the handler.
  private double m_period = 0;

  @Override
  @SuppressWarnings("NoFinalizer")
  protected void finalize() {
    int handle = m_notifier.getAndSet(0);
    NotifierJNI.stopNotifier(handle);
    // Join the thread to ensure the handler has exited.
    if (m_thread.isAlive()) {
      try {
        m_thread.interrupt();
        m_thread.join();
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
    }
    NotifierJNI.cleanNotifier(handle);
  }

  /**
   * Update the alarm hardware to reflect the next alarm.
   */
  private void updateAlarm() {
    int notifier = m_notifier.get();
    if (notifier == 0) {
      return;
    }
    NotifierJNI.updateNotifierAlarm(notifier, (long) (m_expirationTime * 1e6));
  }

  /**
   * Create a Notifier for timer event notification.
   *
   * @param run The handler that is called at the notification time which is set using StartSingle
   *            or StartPeriodic.
   */
  public Notifier(Runnable run) {
    m_handler = run;
    m_notifier.set(NotifierJNI.initializeNotifier());

    m_thread = new Thread(() -> {
      while (!Thread.interrupted()) {
        int notifier = m_notifier.get();
        if (notifier == 0) {
          break;
        }
        long curTime = NotifierJNI.waitForNotifierAlarm(notifier);
        if (curTime == 0) {
          break;
        }

        Runnable handler = null;
        m_processLock.lock();
        try {
          handler = m_handler;
          if (m_periodic) {
            m_expirationTime += m_period;
            updateAlarm();
          }
        } finally {
          m_processLock.unlock();
        }

        if (handler != null) {
          handler.run();
        }
      }
    });
    m_thread.setDaemon(true);
    m_thread.start();
  }

  /**
   * Change the handler function.
   *
   * @param handler Handler
   */
  public void setHandler(Runnable handler) {
    m_processLock.lock();
    try {
      m_handler = handler;
    } finally {
      m_processLock.unlock();
    }
  }

  /**
   * Register for single event notification. A timer event is queued for a single event after the
   * specified delay.
   *
   * @param delay Seconds to wait before the handler is called.
   */
  public void startSingle(double delay) {
    m_processLock.lock();
    try {
      m_periodic = false;
      m_period = delay;
      m_expirationTime = RobotController.getFPGATime() * 1e-6 + delay;
      updateAlarm();
    } finally {
      m_processLock.unlock();
    }
  }

  /**
   * Register for periodic event notification. A timer event is queued for periodic event
   * notification. Each time the interrupt occurs, the event will be immediately requeued for the
   * same time interval.
   *
   * @param period Period in seconds to call the handler starting one period after the call to this
   *               method.
   */
  public void startPeriodic(double period) {
    m_processLock.lock();
    try {
      m_periodic = true;
      m_period = period;
      m_expirationTime = RobotController.getFPGATime() * 1e-6 + period;
      updateAlarm();
    } finally {
      m_processLock.unlock();
    }
  }

  /**
   * Stop timer events from occurring. Stop any repeating timer events from occurring. This will
   * also remove any single notification events from the queue. If a timer-based call to the
   * registered handler is in progress, this function will block until the handler call is complete.
   */
  public void stop() {
    NotifierJNI.cancelNotifierAlarm(m_notifier.get());
  }
}
