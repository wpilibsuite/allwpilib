// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.NotifierJNI;
import edu.wpi.first.units.measure.Time;
import java.util.PriorityQueue;

/**
 * TimedRobot implements the IterativeRobotBase robot program framework.
 *
 * <p>The TimedRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>periodic() functions from the base class are called on an interval by a Notifier instance.
 *
 * <p>The addOneShot() function enqueues an action to be run after a specified fixed delay.
 */
public class TimedRobot extends IterativeRobotBase {

  /** Callback schedule types. */
  enum CallbackScheduleType {
    /** Run the callback indefinitely at a preset interval. */
    PERIODIC {
      @Override
      boolean invoke(Runnable action) {
        action.run();
        return true;
      }
    },
    /** Run the callback only once. */
    ONE_SHOT {
      @Override
      boolean invoke(Runnable action) {
        action.run();
        return false;
      }
    },
    /** Callback is cancelled. Don't run it at all. */
    CANCELLED {
      @Override
      boolean invoke(Runnable action) {
        return false;
      }
    };

    /**
     * Invoke the specified {@code action} if it should run.
     *
     * @param action action to invoke, provided that it should run.
     * @return {@code true} if the action should be rescheduled, {@code false} otherwise.
     */
    abstract boolean invoke(Runnable action);
  }

  public interface Cancellable {
    void cancel();
  }

  /** Holds and invokes the action (a {@link Runnable}) to invoke at a set time. */
  @SuppressWarnings("MemberName")
  static class Callback implements Comparable<Callback>, Cancellable {
    private final Runnable func; // Scheduled action
    public long period; // Time between invocations of a periodic callback
    public long expirationTime; // The next time to invoke the callback
    public volatile CallbackScheduleType m_scheduleType;

    /**
     * Construct a callback container.
     *
     * @param func The callback to run.
     * @param startTimeUs The common starting point for all callback scheduling in microseconds.
     * @param periodUs The period at which to run the callback in microseconds.
     * @param offsetUs The offset from the common starting time in microseconds.
     * @param scheduleType How to schedule the callback. Meaningful options are periodic or
     *     one-shot.
     */
    Callback(
        Runnable func,
        long startTimeUs,
        long periodUs,
        long offsetUs,
        CallbackScheduleType scheduleType) {
      this.func = func;
      this.period = periodUs;
      this.expirationTime =
          startTimeUs
              + offsetUs
              + this.period
              + (RobotController.getFPGATime() - startTimeUs) / this.period * this.period;
      m_scheduleType = scheduleType;
    }

    @Override
    public void cancel() {
      m_scheduleType = CallbackScheduleType.CANCELLED;
    }

    boolean invoke() {
      return m_scheduleType.invoke(func);
    }

    @Override
    public boolean equals(Object rhs) {
      return rhs instanceof Callback callback && expirationTime == callback.expirationTime;
    }

    @Override
    public int hashCode() {
      return Double.hashCode(expirationTime);
    }

    @Override
    public int compareTo(Callback rhs) {
      // Elements with sooner expiration times are sorted as lesser. The head of
      // Java's PriorityQueue is the least element.
      return Long.compare(expirationTime, rhs.expirationTime);
    }
  }

  /** Default loop period. */
  public static final double kDefaultPeriod = 0.02;

  // The C pointer to the notifier object. We don't use it directly, it is
  // just passed to the JNI bindings.
  private final int m_notifier = NotifierJNI.initializeNotifier();

  private final long m_startTimeUs;

  private final PriorityQueue<Callback> m_callbacks = new PriorityQueue<>();

  /** Constructor for TimedRobot. */
  protected TimedRobot() {
    this(kDefaultPeriod);
  }

  /**
   * Constructor for TimedRobot.
   *
   * @param period Period in seconds.
   */
  protected TimedRobot(double period) {
    super(period);
    m_startTimeUs = RobotController.getFPGATime();
    addPeriodic(this::loopFunc, period);
    NotifierJNI.setNotifierName(m_notifier, "TimedRobot");

    HAL.report(tResourceType.kResourceType_Framework, tInstances.kFramework_Timed);
  }

  @Override
  public void close() {
    NotifierJNI.stopNotifier(m_notifier);
    NotifierJNI.cleanNotifier(m_notifier);
  }

  /** Provide an alternate "main loop" via startCompetition(). */
  @Override
  public void startCompetition() {
    robotInit();

    if (isSimulation()) {
      simulationInit();
    }

    // Tell the DS that the robot is ready to be enabled
    System.out.println("********** Robot program startup complete **********");
    DriverStationJNI.observeUserProgramStarting();

    // Loop forever, calling the appropriate mode-dependent function
    while (true) {
      // We don't have to check there's an element in the queue first because
      // there's always at least one (the constructor adds one). If the action
      // runs periodically, it is rescheduled immediately after it runs.
      var callback = m_callbacks.peek();

      NotifierJNI.updateNotifierAlarm(m_notifier, callback.expirationTime);

      long currentTime = NotifierJNI.waitForNotifierAlarm(m_notifier);
      if (currentTime == 0) {
        break;
      }

      // Process all callbacks that are ready to run
      while (m_callbacks.peek().expirationTime <= currentTime) {
        callback = m_callbacks.poll();

        if (callback.invoke()) {

          callback.expirationTime +=
              callback.period
                  + (currentTime - callback.expirationTime) / callback.period * callback.period;
          m_callbacks.add(callback);
        }
      }
    }
  }

  /** Ends the main loop in startCompetition(). */
  @Override
  public void endCompetition() {
    NotifierJNI.stopNotifier(m_notifier);
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param periodSeconds The period at which to run the callback in seconds.
   * @return a {@link Cancellable} that allows the user to cancel periodic invocation.
   */
  public final Cancellable addPeriodic(Runnable callback, double periodSeconds) {
    Callback scheduledCallback =
        new Callback(
            callback,
            m_startTimeUs,
            (long) (periodSeconds * 1e6),
            0,
            CallbackScheduleType.PERIODIC);
    m_callbacks.add(scheduledCallback);
    return scheduledCallback;
  }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param periodSeconds The period at which to run the callback in seconds.
   * @param offsetSeconds The offset from the common starting time in seconds. This is useful for
   *     scheduling a callback in a different timeslot relative to TimedRobot.
   * @return a {@link Cancellable} that allows the user to cancel periodic invocation.
   */
  public final Cancellable addPeriodic(
      Runnable callback, double periodSeconds, double offsetSeconds) {
    Callback scheduledCallback =
        new Callback(
            callback,
            m_startTimeUs,
            (long) (periodSeconds * 1e6),
            (long) (offsetSeconds * 1e6),
            CallbackScheduleType.PERIODIC);
    m_callbacks.add(scheduledCallback);
    return scheduledCallback;
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   * @return a {@link Cancellable} that allows the user to cancel periodic invocation.
   */
  public final Cancellable addPeriodic(Runnable callback, Time period) {
    return addPeriodic(callback, period.in(Seconds));
  }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   * @param offset The offset from the common starting time. This is useful for scheduling a
   *     callback in a different timeslot relative to TimedRobot.
   * @return a {@link Cancellable} that allows the user to cancel periodic invocation.
   */
  public final Cancellable addPeriodic(Runnable callback, Time period, Time offset) {
    return addPeriodic(callback, period.in(Seconds), offset.in(Seconds));
  }

  /**
   * Add a one-shot that, unless cancelled, invokes an action exactly once after a specified delay.
   *
   * @param callback action to run
   * @param delaySeconds the number of seconds to wait before running the action
   * @return a {@link Cancellable} that allows the user to cancel the invocation
   */
  public final Cancellable addOneShot(Runnable callback, double delaySeconds) {
    Callback scheduledCallback =
        new Callback(
            callback, m_startTimeUs, (long) (delaySeconds * 1e6), 0, CallbackScheduleType.ONE_SHOT);
    m_callbacks.add(scheduledCallback);
    return scheduledCallback;
  }
}
