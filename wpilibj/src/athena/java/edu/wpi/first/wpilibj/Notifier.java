/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.lang.Runtime;

import edu.wpi.first.wpilibj.hal.NotifierJNI;
import edu.wpi.first.wpilibj.Utility;

public class Notifier {

  private static class Process implements NotifierJNI.NotifierJNIHandlerFunction {
    // The lock for the process information.
    private ReentrantLock m_processLock = new ReentrantLock();
    // The C pointer to the notifier object. We don't use it directly, it is
    // just passed to the JNI bindings.
    private long m_notifier;
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
    // Lock on the handler so that the handler is not called before it has
    // completed. This is only relevant if the handler takes a very long time
    // to complete (or the period is very short) and when everything is being
    // destructed.
    private ReentrantLock m_handlerLock = new ReentrantLock();

    public Process(Runnable run) {
      m_handler = run;
      m_notifier = NotifierJNI.initializeNotifier(this);
    }

    @Override
    protected void finalize() {
      NotifierJNI.cleanNotifier(m_notifier);
      m_handlerLock.lock();
      m_handlerLock = null;
    }

    /**
     * Update the alarm hardware to reflect the next alarm.
     */
    private void updateAlarm() {
      NotifierJNI.updateNotifierAlarm(m_notifier, (long) (m_expirationTime * 1e6));
    }

    /**
     * Handler which is called by the HAL library; it handles the subsequent
     * calling of the user handler.
     */
    @Override
    public void apply(long time) {
      m_processLock.lock();
      if (m_periodic) {
        m_expirationTime += m_period;
        updateAlarm();
      }

      m_handlerLock.lock();
      m_processLock.unlock();

      m_handler.run();
      m_handlerLock.unlock();
    }

    public void start(double period, boolean periodic) {
      synchronized (m_processLock) {
        m_periodic = periodic;
        m_period = period;
        m_expirationTime = Utility.getFPGATime() * 1e-6 + m_period;
        updateAlarm();
      }
    }

    public void stop() {
      NotifierJNI.stopNotifierAlarm(m_notifier);

      // Wait for a currently executing handler to complete before returning
      // from stop()
      m_handlerLock.lock();
      m_handlerLock.unlock();
    }
  }

  private Process m_process;

  /**
   * Create a Notifier for timer event notification.
   *
   * @param run The handler that is called at the notification time which is set
   *        using StartSingle or StartPeriodic.
   */
  public Notifier(Runnable run) {
    m_process = new Process(run);
  }

  /**
   * Register for single event notification. A timer event is queued for a
   * single event after the specified delay.
   *
   * @param delay Seconds to wait before the handler is called.
   */
  public void startSingle(double delay) {
    m_process.start(delay, false);
  }

  /**
   * Register for periodic event notification. A timer event is queued for
   * periodic event notification. Each time the interrupt occurs, the event will
   * be immediately requeued for the same time interval.
   *
   * @param period Period in seconds to call the handler starting one period
   *        after the call to this method.
   */
  public void startPeriodic(double period) {
    m_process.start(period, true);
  }

  /**
   * Stop timer events from occuring. Stop any repeating timer events from
   * occuring. This will also remove any single notification events from the
   * queue. If a timer-based call to the registered handler is in progress, this
   * function will block until the handler call is complete.
   */
  public void stop() {
    m_process.stop();
  }
}
