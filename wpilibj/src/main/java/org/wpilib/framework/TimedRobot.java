// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import static org.wpilib.units.Units.Seconds;

import org.wpilib.driverstation.DriverStation;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.NotifierJNI;
import org.wpilib.internal.PeriodicPriorityQueue;
import org.wpilib.system.RobotController;
import org.wpilib.units.measure.Frequency;
import org.wpilib.units.measure.Time;

/**
 * TimedRobot implements the IterativeRobotBase robot program framework.
 *
 * <p>The TimedRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>periodic() functions from the base class are called on an interval by a Notifier instance.
 */
public class TimedRobot extends IterativeRobotBase {
  /** Default loop period. */
  @SuppressWarnings("MemberName")
  public static final double DEFAULT_PERIOD = 0.02;

  // The C pointer to the notifier object. We don't use it directly, it is
  // just passed to the JNI bindings.
  private final int m_notifier = NotifierJNI.createNotifier();

  private final long m_startTimeUs;

  private final PeriodicPriorityQueue m_callbackQueue = new PeriodicPriorityQueue();

  /** Constructor for TimedRobot. */
  protected TimedRobot() {
    this(DEFAULT_PERIOD);
  }

  /**
   * Constructor for TimedRobot.
   *
   * @param period The period of the robot loop function.
   */
  @SuppressWarnings("this-escape")
  protected TimedRobot(double period) {
    super(period);
    m_startTimeUs = RobotController.getMonotonicTime();
    addPeriodic(this::loopFunc, period);
    NotifierJNI.setNotifierName(m_notifier, "TimedRobot");

    HAL.reportUsage("Framework", "TimedRobot");
  }

  /**
   * Constructor for TimedRobot.
   *
   * @param period The period of the robot loop function.
   */
  protected TimedRobot(Time period) {
    this(period.in(Seconds));
  }

  /**
   * Constructor for TimedRobot.
   *
   * @param frequency The frequency of the robot loop function.
   */
  protected TimedRobot(Frequency frequency) {
    this(frequency.asPeriod());
  }

  @Override
  public void close() {
    NotifierJNI.destroyNotifier(m_notifier);
  }

  /** Provide an alternate "main loop" via startCompetition(). */
  @Override
  public void startCompetition() {
    if (isSimulation()) {
      simulationInit();
    }

    // Tell the DS that the robot is ready to be enabled
    System.out.println("********** Robot program startup complete **********");
    DriverStation.observeUserProgramStarting();

    // Loop forever, calling the appropriate mode-dependent function
    while (true) {
      if (!m_callbackQueue.runCallbacks(m_notifier)) {
        break;
      }
    }
  }

  /** Ends the main loop in startCompetition(). */
  @Override
  public void endCompetition() {
    NotifierJNI.destroyNotifier(m_notifier);
  }

  /**
   * Return the system clock time in microseconds for the start of the current periodic loop. This
   * is in the same time base as Timer.getMonotonicTimestamp(), but is stable through a loop. It is
   * updated at the beginning of every periodic callback (including the normal periodic loop).
   *
   * @return Robot running time in microseconds, as of the start of the current periodic function.
   */
  public long getLoopStartTime() {
    return m_callbackQueue.getLoopStartTime();
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback in seconds.
   */
  public final void addPeriodic(Runnable callback, double period) {
    addPeriodic(callback, period, period);
  }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * <p>This is scheduled on TimedRobot's Notifier, so TimedRobot and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback in seconds.
   * @param offset The offset from the common starting time in seconds. This is useful for
   *     scheduling a callback in a different timeslot relative to TimedRobot.
   */
  public final void addPeriodic(Runnable callback, double period, double offset) {
    m_callbackQueue.add(callback, m_startTimeUs, period, offset);
  }
}
