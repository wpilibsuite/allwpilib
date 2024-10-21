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
import edu.wpi.first.wpilibj.Tracer.SubstitutiveTracer;
import java.util.Optional;
import java.util.PriorityQueue;

/**
 * TimedRobot implements the IterativeRobotBase robot program framework.
 *
 * <p>The TimedRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>periodic() functions from the base class are called on an interval by a Notifier instance.
 */
public class TimedRobot extends IterativeRobotBase {
  /** A container for a callback to run at a specific period. */
  static class Callback implements Comparable<Callback> {
    /** The callback to run. */
    public Runnable m_func;

    /**
     * The tracer to use for this callback. This allows callbacks to appear in their own tracer
     * table separate from the main RobotLoop tracer table.
     */
    public Optional<SubstitutiveTracer> m_tracerSub;

    /** The period at which to run the callback in microseconds. */
    public long m_period;

    /** The time at which the callback should be run. */
    public long m_expirationTime;

    /**
     * Construct a callback container.
     *
     * @param func The callback to run.
     * @param name The name of the callback. This is used to create a separate tracer table for the
     *     callback. If empty, separate tracer tables are not used.
     * @param startTimeUs The common starting time in microseconds.
     * @param periodUs The period at which to run the callback in microseconds.
     * @param offsetUs The offset from the common starting time in microseconds. This is useful for
     *     scheduling a callback in a different timeslot relative to TimedRobot.
     */
    Callback(Runnable func, String name, long startTimeUs, long periodUs, long offsetUs) {
      this.m_func = func;
      this.m_tracerSub = Optional.ofNullable(name).map(SubstitutiveTracer::new);
      this.m_period = periodUs;
      this.m_expirationTime =
          startTimeUs
              + offsetUs
              + this.m_period
              + (RobotController.getFPGATime() - startTimeUs) / this.m_period * this.m_period;
    }

    void call() {
      m_tracerSub.ifPresentOrElse(tracer -> tracer.subWith(m_func), m_func);
    }

    @Override
    public boolean equals(Object rhs) {
      return rhs instanceof Callback callback && m_expirationTime == callback.m_expirationTime;
    }

    @Override
    public int hashCode() {
      return Double.hashCode(m_expirationTime);
    }

    @Override
    public int compareTo(Callback rhs) {
      // Elements with sooner expiration times are sorted as lesser. The head of
      // Java's PriorityQueue is the least element.
      return Long.compare(m_expirationTime, rhs.m_expirationTime);
    }
  }

  /** Default loop period. */
  public static final double kDefaultPeriod = 0.02;

  // The C pointer to the notifier object. We don't use it directly, it is
  // just passed to the JNI bindings.
  private final int m_notifier = NotifierJNI.initializeNotifier();

  private long m_startTimeUs;

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
    addPeriodic(this::loopFunc, null, period);
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
      // there's always at least one (the constructor adds one). It's reenqueued
      // at the end of the loop.
      var callback = m_callbacks.poll();

      NotifierJNI.updateNotifierAlarm(m_notifier, callback.m_expirationTime);

      long currentTime = NotifierJNI.waitForNotifierAlarm(m_notifier);
      if (currentTime == 0) {
        break;
      }

      callback.call();

      // Increment the expiration time by the number of full periods it's behind
      // plus one to avoid rapid repeat fires from a large loop overrun. We
      // assume currentTime ≥ expirationTime rather than checking for it since
      // the callback wouldn't be running otherwise.
      callback.m_expirationTime +=
          callback.m_period
              + (currentTime - callback.m_expirationTime) / callback.m_period * callback.m_period;
      m_callbacks.add(callback);

      // Process all other callbacks that are ready to run
      while (m_callbacks.peek().m_expirationTime <= currentTime) {
        callback = m_callbacks.poll();

        callback.call();

        callback.m_expirationTime +=
            callback.m_period
                + (currentTime - callback.m_expirationTime) / callback.m_period * callback.m_period;
        m_callbacks.add(callback);
      }
    }
  }

  /** Ends the main loop in startCompetition(). */
  @Override
  public void endCompetition() {
    NotifierJNI.stopNotifier(m_notifier);
  }

  private static int m_periodicCount;

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param periodSeconds The period at which to run the callback in seconds.
   * @deprecated Use {@link #addPeriodic(Runnable, String, double)} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public final void addPeriodic(Runnable callback, double periodSeconds) {
    addPeriodic(
        callback,
        "Periodic" + m_periodicCount++,
        Time.ofBaseUnits(periodSeconds, Seconds),
        Time.ofBaseUnits(0.0, Seconds));
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
   * @deprecated Use {@link #addPeriodic(Runnable, String, double, double)} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public final void addPeriodic(Runnable callback, double periodSeconds, double offsetSeconds) {
    addPeriodic(
        callback,
        "Periodic" + m_periodicCount++,
        Time.ofBaseUnits(periodSeconds, Seconds),
        Time.ofBaseUnits(offsetSeconds, Seconds));
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   * @deprecated Use {@link #addPeriodic(Runnable, String, Time)} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public final void addPeriodic(Runnable callback, Time period) {
    addPeriodic(callback, "Periodic" + m_periodicCount++, period, Time.ofBaseUnits(0.0, Seconds));
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
   * @deprecated Use {@link #addPeriodic(Runnable, String, Time, Time)} instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public final void addPeriodic(Runnable callback, Time period, Time offset) {
    addPeriodic(callback, "Periodic" + m_periodicCount++, period, offset);
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param name The name of the callback. This is used to create a separate tracer table for the
   *     callback.
   * @param period The period at which to run the callback in seconds.
   * @param offset The offset from the common starting time in seconds. This is useful for
   *     scheduling a callback in a different timeslot relative to TimedRobot.
   */
  public final void addPeriodic(Runnable callback, String name, double period, double offset) {
    m_callbacks.add(
        new Callback(
            callback,
            name,
            m_startTimeUs,
            (long) (period * 1_000_000),
            (long) (offset * 1_000_000)));
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param name The name of the callback. This is used to create a separate tracer table for the
   *     callback.
   * @param period The period at which to run the callback in seconds.
   */
  public final void addPeriodic(Runnable callback, String name, double period) {
    addPeriodic(callback, name, period, 0.0);
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param name The name of the callback. This is used to create a separate tracer table for the
   *     callback.
   * @param period The period at which to run the callback.
   * @param offset The offset from the common starting time. This is useful for scheduling a
   *     callback in a different timeslot relative to TimedRobot.
   */
  public final void addPeriodic(Runnable callback, String name, Time period, Time offset) {
    addPeriodic(callback, name, period.in(Seconds), offset.in(Seconds));
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param name The name of the callback. This is used to create a separate tracer table for the
   *     callback.
   * @param period The period at which to run the callback.
   */
  public final void addPeriodic(Runnable callback, String name, Time period) {
    addPeriodic(callback, name, period, Seconds.zero());
  }
}
