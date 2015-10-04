package edu.wpi.first.wpilibj;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.lang.Runtime;

import edu.wpi.first.wpilibj.hal.NotifierJNI;
import edu.wpi.first.wpilibj.Utility;

public class Notifier {

  static private class ProcessQueue implements Runnable {
    public void run() {
      Notifier current;
      while (true) {
        Notifier.queueLock.lock();

        double currentTime = Utility.getFPGATime() * 1e-6;
        current = Notifier.timerQueueHead;

        if (current == null || current.m_expirationTime > currentTime) {
          Notifier.queueLock.unlock();
          break;
        }

        Notifier.timerQueueHead = current.m_nextEvent;

        if (current.m_periodic) {
          current.insertInQueue(true);
        } else {
          current.m_queued = false;
        }

        current.m_handlerLock.lock();
        Notifier.queueLock.unlock();

        current.m_handler.run();
        current.m_handlerLock.unlock();
      }


      Notifier.queueLock.lock();
      Notifier.updateAlarm();
      Notifier.queueLock.unlock();
    }

  }

  // Maximum time, in seconds, that the FPGA returns before rolling over to 0.
  static private final double kRolloverTime = (1l << 32) / 1e6;
  // Number of instances of Notifier classes created, so that we can call
  // cleanNotifier() after all the Notifiers are stopped.
  static private int refcount = 0;
  // The next Notifier instance which needs to be called.
  static private Notifier timerQueueHead = null;
  // The C pointer to the notifier object. We don't use it directly, it is just
  // passed to the JNI bindings.
  private static long m_notifier;
  // The lock for the queue information (namely, timerQueueHead and the
  // m_nextEvent members).
  private static ReentrantLock queueLock = new ReentrantLock();
  // The handler which is called by the HAL library; it handles the subsequent
  // calling of the user handlers.
  // This is the only Runnable actually passed to the JNI bindings.
  private static ProcessQueue m_processQueue;

  // The next Notifier whose handler will need to be called after this one.
  private Notifier m_nextEvent = null;
  // The time, in microseconds, at which the corresponding handler should be
  // called. Has the same zero as Utility.getFPGATime().
  private double m_expirationTime = 0;
  // The handler passed in by the user which should be called at the appropriate
  // interval.
  private Runnable m_handler;
  // Whether we are calling the handler just once or periodically.
  private boolean m_periodic = false;
  // If periodic, the period of the calling; if just once, stores how long it
  // is until we call the handler.
  private double m_period = 0;
  // Whether we are currently queued to be called at m_expirationTime.
  private boolean m_queued = false;
  // Lock on the handler so that the handler is not called before it has
  // completed. This is only relevant if the handler takes a very long time to
  // complete (or the period is very short) and when everything is being
  // destructed.
  private ReentrantLock m_handlerLock = new ReentrantLock();

  /**
   * Create a Notifier for timer event notification.
   *$
   * @param run The handler that is called at the notification time which is set
   *        using StartSingle or StartPeriodic.
   */
  public Notifier(Runnable run) {
    if (refcount == 0) {
      init();
    }
    refcount += 1;
    m_handler = run;
  }

  protected void finalize() {
    queueLock.lock();

    deleteFromQueue();

    // If this was the last instance of a Notifier, clean up after ourselves.
    if ((--refcount) == 0) {
      NotifierJNI.cleanNotifier(m_notifier);
    }

    queueLock.unlock();

    m_handlerLock.lock();
    m_handlerLock = null;
  }

  /**
   * Update the alarm hardware to reflect the current first element in the
   * queue. Compute the time the next alarm should occur based on the current
   * time and the period for the first element in the timer queue. WARNING: this
   * method does not do synchronization! It must be called from somewhere that
   * is taking care of synchronizing access to the queue.
   */
  static protected void updateAlarm() {
    if (timerQueueHead != null) {
      NotifierJNI.updateNotifierAlarm(m_notifier, (int) (timerQueueHead.m_expirationTime * 1e6));
    }
  }

  /**
   * Insert this Notifier into the timer queue in right place. WARNING: this
   * method does not do synchronization! It must be called from somewhere that
   * is taking care of synchronizing access to the queue.
   *$
   * @param reschedule If false, the scheduled alarm is based on the current
   *        time and UpdateAlarm method is called which will enable the alarm if
   *        necessary. If true, update the time by adding the period (no drift)
   *        when rescheduled periodic from ProcessQueue. This ensures that the
   *        public methods only update the queue after finishing inserting.
   */
  protected void insertInQueue(boolean reschedule) {
    if (reschedule) {
      m_expirationTime += m_period;
    } else {
      m_expirationTime = Utility.getFPGATime() * 1e-6 + m_period;
    }

    if (m_expirationTime > kRolloverTime) {
      m_expirationTime -= kRolloverTime;
    }

    if (timerQueueHead == null || timerQueueHead.m_expirationTime >= this.m_expirationTime) {
      // the queue is empty or greater than the new entry
      // the new entry becomes the first element
      this.m_nextEvent = timerQueueHead;
      timerQueueHead = this;

      if (!reschedule) {
        // since the first element changed, update alarm, unless we already plan
        // to
        updateAlarm();
      }
    } else {
      for (Notifier n = timerQueueHead;; n = n.m_nextEvent) {
        if (n.m_nextEvent == null || n.m_nextEvent.m_expirationTime > this.m_expirationTime) {
          this.m_nextEvent = n.m_nextEvent;
          n.m_nextEvent = this;
          break;
        }
      }
    }

    m_queued = true;
  }

  /**
   * Delete this Notifier from the timer queue. WARNING: this method does not do
   * synchronization! It must be called from somewhere that is taking care of
   * synchronizing access to the queue. Remove this Notifier from the timer
   * queue and adjust the next interrupt time to reflect the current top of the
   * queue.
   */
  private void deleteFromQueue() {
    if (m_queued) {
      m_queued = false;
      assert (timerQueueHead != null);
      if (timerQueueHead == this) {
        // removing the first item in the list - update the alarm
        timerQueueHead = this.m_nextEvent;
        updateAlarm();
      } else {
        for (Notifier n = timerQueueHead; n != null; n = n.m_nextEvent) {
          if (n.m_nextEvent == this) {
            // this element is the next element from *n from the queue
            // Point n around this.
            n.m_nextEvent = this.m_nextEvent;
          }
        }
      }
    }
  }

  /**
   * Register for single event notification. A timer event is queued for a
   * single event after the specified delay.
   *$
   * @param delay Seconds to wait before the handler is called.
   */
  public void startSingle(double delay) {
    queueLock.lock();
    m_periodic = false;
    m_period = delay;
    deleteFromQueue();
    insertInQueue(false);
    queueLock.unlock();
  }

  /**
   * Register for periodic event notification. A timer event is queued for
   * periodic event notification. Each time the interrupt occurs, the event will
   * be immediately requeued for the same time interval.
   *$
   * @param period Period in seconds to call the handler starting one period
   *        after the call to this method.
   */
  public void startPeriodic(double period) {
    queueLock.lock();
    m_periodic = true;
    m_period = period;
    deleteFromQueue();
    insertInQueue(false);
    queueLock.unlock();
  }

  /**
   * Stop timer events from occuring. Stop any repeating timer events from
   * occuring. This will also remove any single notification events from the
   * queue. If a timer-based call to the registered handler is in progress, this
   * function will block until the handler call is complete.
   */
  public void stop() {
    queueLock.lock();
    deleteFromQueue();
    queueLock.unlock();

    // Wait for a currently executing handler to complete before returning from
    // stop()
    m_handlerLock.lock();
    m_handlerLock.unlock();
  }

  // First time init.
  protected static void init() {
    m_processQueue = new ProcessQueue();
    m_notifier = NotifierJNI.initializeNotifier(m_processQueue);
  }
}
