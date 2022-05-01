// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.NotifierJNI;
import java.util.PriorityQueue;

/**
 * TimedRobot implements the IterativeRobotBase robot program framework.
 *
 * <p>The TimedRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>periodic() functions from the base class are called on an interval by a Notifier instance.
 */
public class TimedRobot extends IterativeRobotBase {
  @SuppressWarnings("MemberName")
  static class Callback implements Comparable<Callback> {
    public Runnable func;
    public double period;
    public double expirationTime;

    /**
     * Construct a callback container.
     *
     * @param func The callback to run.
     * @param startTimeSeconds The common starting point for all callback scheduling in seconds.
     * @param periodSeconds The period at which to run the callback in seconds.
     * @param offsetSeconds The offset from the common starting time in seconds.
     */
    Callback(Runnable func, double startTimeSeconds, double periodSeconds, double offsetSeconds) {
      this.func = func;
      this.period = periodSeconds;
      this.expirationTime =
          startTimeSeconds
              + offsetSeconds
              + Math.floor((Timer.getFPGATimestamp() - startTimeSeconds) / this.period)
                  * this.period
              + this.period;
    }

    @Override
    public boolean equals(Object rhs) {
      if (rhs instanceof Callback) {
        return Double.compare(expirationTime, ((Callback) rhs).expirationTime) == 0;
      }
      return false;
    }

    @Override
    public int hashCode() {
      return Double.hashCode(expirationTime);
    }

    @Override
    public int compareTo(Callback rhs) {
      // Elements with sooner expiration times are sorted as lesser. The head of
      // Java's PriorityQueue is the least element.
      return Double.compare(expirationTime, rhs.expirationTime);
    }
  }

  public static final double kDefaultPeriod = 0.02;

  // The C pointer to the notifier object. We don't use it directly, it is
  // just passed to the JNI bindings.
  private final int m_notifier = NotifierJNI.initializeNotifier();

  private double m_startTime;

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
    m_startTime = Timer.getFPGATimestamp();
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
  @SuppressWarnings("UnsafeFinalization")
  public void startCompetition() {
    robotInit();

    if (isSimulation()) {
      simulationInit();
    }

    // Tell the DS that the robot is ready to be enabled
    System.out.println("********** Robot program startup complete **********");
    HAL.observeUserProgramStarting();

    // Loop forever, calling the appropriate mode-dependent function
    while (true) {
      // We don't have to check there's an element in the queue first because
      // there's always at least one (the constructor adds one). It's reenqueued
      // at the end of the loop.
      var callback = m_callbacks.poll();

      NotifierJNI.updateNotifierAlarm(m_notifier, (long) (callback.expirationTime * 1e6));

      long curTime = NotifierJNI.waitForNotifierAlarm(m_notifier);
      if (curTime == 0) {
        break;
      }

      callback.func.run();

      callback.expirationTime += callback.period;
      m_callbacks.add(callback);

      // Process all other callbacks that are ready to run
      while ((long) (m_callbacks.peek().expirationTime * 1e6) <= curTime) {
        callback = m_callbacks.poll();

        callback.func.run();

        callback.expirationTime += callback.period;
        m_callbacks.add(callback);
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
   */
  public void addPeriodic(Runnable callback, double periodSeconds) {
    m_callbacks.add(new Callback(callback, m_startTime, periodSeconds, 0.0));
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
   */
  public void addPeriodic(Runnable callback, double periodSeconds, double offsetSeconds) {
    m_callbacks.add(new Callback(callback, m_startTime, periodSeconds, offsetSeconds));
  }
}
