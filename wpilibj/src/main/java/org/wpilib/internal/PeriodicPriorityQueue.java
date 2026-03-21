// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.internal;

import java.util.Collection;
import java.util.PriorityQueue;
import org.wpilib.hardware.hal.NotifierJNI;
import org.wpilib.system.RobotController;
import org.wpilib.util.WPIUtilJNI;

/**
 * A priority queue for scheduling periodic callbacks based on their next execution time.
 *
 * <p>This class manages a collection of periodic callbacks that execute at specified intervals.
 * Callbacks are scheduled using FPGA timestamps and automatically rescheduled after execution to
 * maintain their periodic behavior. The queue uses a priority heap to efficiently determine the
 * next callback to execute.
 *
 * <p>This is an internal scheduling primitive used by robot frameworks like TimedRobot.
 */
public class PeriodicPriorityQueue {
  /** Internal priority queue ordered by callback expiration times. */
  private final PriorityQueue<Callback> m_queue;

  /** Constructs an empty callback queue. */
  public PeriodicPriorityQueue() {
    m_queue = new PriorityQueue<>();
  }

  /**
   * Adds a periodic callback to the queue with a specified start time.
   *
   * @param func The function to call periodically.
   * @param timestamp The common starting point for callback scheduling in FPGA timestamp
   *     microseconds.
   * @param periodSeconds The callback period in seconds.
   * @param offsetSeconds The offset from the common starting time in seconds.
   */
  public void add(Runnable func, long timestamp, double periodSeconds, double offsetSeconds) {
    m_queue.add(new Callback(func, timestamp, periodSeconds, offsetSeconds));
  }

  /**
   * Adds a periodic callback to the queue with a specified start time.
   *
   * @param func The function to call periodically.
   * @param timestamp The common starting point for callback scheduling in FPGA timestamp
   *     microseconds.
   * @param periodSeconds The callback period in seconds.
   */
  public void add(Runnable func, long timestamp, double periodSeconds) {
    m_queue.add(new Callback(func, timestamp, periodSeconds));
  }

  /**
   * Adds a periodic callback to the queue, starting from the current FPGA time.
   *
   * @param func The function to call periodically.
   * @param periodSeconds The callback period in seconds.
   * @param offsetSeconds The offset from the current FPGA time in seconds.
   */
  public void add(Runnable func, double periodSeconds, double offsetSeconds) {
    add(func, RobotController.getMonotonicTime(), periodSeconds, offsetSeconds);
  }

  /**
   * Adds a periodic callback to the queue, starting from the current FPGA time.
   *
   * @param func The function to call periodically.
   * @param periodSeconds The callback period in seconds.
   */
  public void add(Runnable func, double periodSeconds) {
    add(func, RobotController.getMonotonicTime(), periodSeconds);
  }

  /**
   * Adds a pre-constructed callback to the queue.
   *
   * @param callback The callback to add.
   */
  public void add(Callback callback) {
    m_queue.add(callback);
  }

  /**
   * Adds multiple callbacks to the queue.
   *
   * @param callbacks The collection of callbacks to add.
   */
  public void addAll(Collection<Callback> callbacks) {
    m_queue.addAll(callbacks);
  }

  /**
   * Removes all callbacks associated with the given function.
   *
   * @param func The function whose callbacks should be removed.
   */
  public void remove(Runnable func) {
    m_queue.removeIf(callback -> callback.m_func.equals(func));
  }

  /**
   * Removes a specific callback from the queue.
   *
   * @param callback The callback to remove.
   */
  public void remove(Callback callback) {
    m_queue.remove(callback);
  }

  /**
   * Removes multiple callbacks from the queue.
   *
   * @param callbacks The collection of callbacks to remove.
   */
  public void removeAll(Collection<Callback> callbacks) {
    m_queue.removeAll(callbacks);
  }

  /** Removes all callbacks from the queue. */
  public void clear() {
    m_queue.clear();
  }

  /**
   * Executes all callbacks that are due, then waits for the next callback's scheduled time.
   *
   * <p>This method performs the following steps:
   *
   * <ol>
   *   <li>Retrieves the callback with the earliest expiration time from the queue
   *   <li>Sets a hardware notifier alarm to wait until that callback's expiration time
   *   <li>Blocks until the notifier signals or is interrupted
   *   <li>Executes the callback and reschedules it for its next period
   *   <li>Processes any additional callbacks that have become due during execution
   * </ol>
   *
   * <p>When rescheduling callbacks, this method automatically compensates for execution delays by
   * advancing the expiration time by the number of full periods that have elapsed, ensuring
   * callbacks maintain their scheduled phase over time.
   *
   * @param notifier The HAL notifier handle to use for timing.
   * @throws InterruptedException if the wait is interrupted, indicating the callback loop should
   *     exit.
   * @throws IllegalStateException if the queue is empty when this method is called.
   */
  public void runCallbacks(int notifier) throws InterruptedException {
    // We don't have to check there's an element in the queue first because
    // there's always at least one (the constructor adds one). It's reenqueued
    // at the end of the loop.
    var callback = m_queue.poll();
    if (callback == null) {
      throw new IllegalStateException(
          "TimedRobot callback queue is empty (something is very wrong)");
    }

    NotifierJNI.setNotifierAlarm(notifier, callback.m_expirationTime, 0, true, true);

    try {
      WPIUtilJNI.waitForObject(notifier);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      throw ex;
    }

    long currentTime = RobotController.getMonotonicTime();

    callback.m_func.run();

    // Increment the expiration time by the number of full periods it's behind
    // plus one to avoid rapid repeat fires from a large loop overrun. We
    // assume currentTime ≥ expirationTime rather than checking for it since
    // the callback wouldn't be running otherwise.
    callback.m_expirationTime +=
        callback.m_period
            + (currentTime - callback.m_expirationTime) / callback.m_period * callback.m_period;
    m_queue.add(callback);

    // Process all other callbacks that are ready to run
    while (m_queue.peek().m_expirationTime <= currentTime) {
      callback = m_queue.poll();

      callback.m_func.run();

      callback.m_expirationTime +=
          callback.m_period
              + (currentTime - callback.m_expirationTime) / callback.m_period * callback.m_period;
      m_queue.add(callback);
    }
  }

  /**
   * A periodic callback with scheduling metadata.
   *
   * <p>Each callback tracks its target function, period, and next expiration time. After execution,
   * the expiration time is automatically advanced by full periods to maintain precise timing even
   * when execution is delayed.
   */
  public static class Callback implements Comparable<Callback> {
    /** The function to execute when the callback fires. */
    public final Runnable m_func;

    /** The period at which to run the callback in microseconds. */
    public final long m_period;

    /** The next scheduled execution time in FPGA timestamp microseconds. */
    public long m_expirationTime;

    /**
     * Construct a callback container.
     *
     * @param func The callback to run.
     * @param startTime The common starting point for all callback scheduling in microseconds.
     * @param period The period at which to run the callback in microseconds.
     * @param offset The offset from the common starting time in microseconds.
     */
    public Callback(Runnable func, long startTime, long period, long offset) {
      this.m_func = func;
      this.m_period = period;
      this.m_expirationTime =
          startTime
              + offset
              + (1 + (RobotController.getMonotonicTime() - startTime - offset) / this.m_period)
                  * this.m_period;
    }

    /**
     * Construct a callback container.
     *
     * @param func The callback to run.
     * @param timestamp The common starting point for all callback scheduling in microseconds.
     * @param periodSeconds The period at which to run the callback in seconds.
     * @param offsetSeconds The offset from the common starting time in seconds.
     */
    public Callback(Runnable func, long timestamp, double periodSeconds, double offsetSeconds) {
      this(func, timestamp, (long) (periodSeconds * 1e6), (long) (offsetSeconds * 1e6));
    }

    /**
     * Construct a callback container.
     *
     * @param func The callback to run.
     * @param timestamp The common starting point for all callback scheduling in microseconds.
     * @param periodSeconds The period at which to run the callback in seconds.
     */
    public Callback(Runnable func, long timestamp, double periodSeconds) {
      this(func, timestamp, (long) (periodSeconds * 1e6), 0);
    }

    /**
     * Compares callbacks based on expiration time for equality.
     *
     * @param rhs The object to compare against.
     * @return true if rhs is a Callback with the same expiration time.
     */
    @Override
    public boolean equals(Object rhs) {
      return rhs instanceof Callback callback
          && m_func == callback.m_func
          && m_expirationTime == callback.m_expirationTime;
    }

    /**
     * Returns a hash code based on the expiration time.
     *
     * @return hash code for this callback.
     */
    @Override
    public int hashCode() {
      return Long.hashCode(m_expirationTime);
    }

    /**
     * Compares this callback to another based on expiration time.
     *
     * <p>Callbacks with earlier expiration times are considered "less than" those with later
     * expiration times. This ordering is used by the priority queue to determine execution order.
     *
     * @param rhs The callback to compare to.
     * @return negative if this expires before rhs, positive if after, zero if equal.
     */
    @Override
    public int compareTo(Callback rhs) {
      // Elements with sooner expiration times are sorted as lesser. The head of
      // Java's PriorityQueue is the least element.
      return Long.compare(m_expirationTime, rhs.m_expirationTime);
    }
  }
}
