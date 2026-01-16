// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.opmode;

import static org.wpilib.units.Units.Seconds;

import java.util.PriorityQueue;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.framework.OpModeRobot;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.NotifierJNI;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.system.RobotController;
import org.wpilib.system.Watchdog;
import org.wpilib.units.measure.Time;
import org.wpilib.util.WPIUtilJNI;

/**
 * An opmode structure for periodic operation. This base class implements a loop that runs one or
 * more functions periodically (on a set time interval aka loop period). The primary periodic
 * callback function is the abstract periodic() function; the time interval for this callback is 20
 * ms by default, but may be changed via passing a different time interval to the constructor.
 * Additional periodic callbacks with different intervals can be added using the addPeriodic() set
 * of functions.
 *
 * <p>Lifecycle:
 *
 * <ul>
 *   <li>constructed when opmode selected on driver station
 *   <li>disabledPeriodic() called periodically as long as DS is disabled. Note this is not called
 *       on a set time interval (it does not use the same time interval as periodic())
 *   <li>when DS transitions from disabled to enabled, start() is called once
 *   <li>while DS is enabled, periodic() is called periodically on the time interval set by the
 *       constructor, and additional periodic callbacks added via addPeriodic() are called
 *       periodically on their set time intervals
 *   <li>when DS transitions from enabled to disabled, or a different opmode is selected on the
 *       driver station when the DS is enabled, end() is called, followed by close(); the object is
 *       not reused
 *   <li>if a different opmode is selected on the driver station when the DS is disabled, only
 *       close() is called; the object is not reused
 * </ul>
 */
public abstract class PeriodicOpMode implements OpMode {
  @SuppressWarnings("MemberName")
  static class Callback implements Comparable<Callback> {
    public Runnable func;
    public long period;
    public long expirationTime;

    /**
     * Construct a callback container.
     *
     * @param func The callback to run.
     * @param startTime The common starting point for all callback scheduling in microseconds.
     * @param period The period at which to run the callback in microseconds.
     * @param offset The offset from the common starting time in microseconds.
     */
    Callback(Runnable func, long startTime, long period, long offset) {
      this.func = func;
      this.period = period;
      this.expirationTime =
          startTime
              + offset
              + this.period
              + (RobotController.getFPGATime() - startTime) / this.period * this.period;
    }

    @Override
    public boolean equals(Object rhs) {
      return rhs instanceof Callback callback && expirationTime == callback.expirationTime;
    }

    @Override
    public int hashCode() {
      return Long.hashCode(expirationTime);
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
  private int m_notifier = NotifierJNI.createNotifier();

  private long m_startTimeUs;
  private long m_loopStartTimeUs;

  private final ControlWord m_word = new ControlWord();
  private final double m_period;
  private final Watchdog m_watchdog;

  private long m_opModeId;
  private boolean m_running = true;

  private final PriorityQueue<Callback> m_callbacks = new PriorityQueue<>();
  private final OpModeRobot m_robot;

  /**
   * Constructor. Periodic opmodes may specify the period used for the periodic() function; the
   * single-argument constructor uses a default period of 20 ms.
   *
   * @param robot robot instance
   */
  protected PeriodicOpMode(OpModeRobot robot) {
    this(robot, kDefaultPeriod);
  }

  /**
   * Constructor. Periodic opmodes may specify the period used for the periodic() function.
   *
   * @param robot robot instance
   * @param period period (in seconds) for callbacks to the periodic() function
   */
  protected PeriodicOpMode(OpModeRobot robot, double period) {
    m_startTimeUs = RobotController.getFPGATime();
    m_period = period;
    m_watchdog = new Watchdog(period, this::printLoopOverrunMessage);

    m_robot = robot;

    addPeriodic(this::loopFunc, period);
    NotifierJNI.setNotifierName(m_notifier, "PeriodicOpMode");

    HAL.reportUsage("OpMode", "PeriodicOpMode");
  }

  /** Called periodically while the opmode is selected on the DS (robot is disabled). */
  @Override
  public void disabledPeriodic() {}

  /**
   * Called when the opmode is de-selected on the DS. The object is not reused even if the same
   * opmode is selected again (a new object will be created).
   */
  public void close() {}

  /**
   * Called a single time when the robot transitions from disabled to enabled. This is called prior
   * to periodic() being called.
   */
  public void start() {}

  /** Called periodically while the robot is enabled. */
  public abstract void periodic();

  /**
   * Called a single time when the robot transitions from enabled to disabled, or just before
   * close() is called if a different opmode is selected while the robot is enabled.
   */
  public void end() {}

  /**
   * Return the system clock time in micrseconds for the start of the current periodic loop. This is
   * in the same time base as Timer.getFPGATimestamp(), but is stable through a loop. It is updated
   * at the beginning of every periodic callback (including the normal periodic loop).
   *
   * @return Robot running time in microseconds, as of the start of the current periodic function.
   */
  public long getLoopStartTime() {
    return m_loopStartTimeUs;
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on the same Notifier as periodic(), so periodic() and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback in seconds.
   */
  public final void addPeriodic(Runnable callback, double period) {
    m_callbacks.add(new Callback(callback, m_startTimeUs, (long) (period * 1e6), 0));
  }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * <p>This is scheduled on the same Notifier as periodic(), so periodic() and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback in seconds.
   * @param offset The offset from the common starting time in seconds. This is useful for
   *     scheduling a callback in a different timeslot relative to PeriodicOpMode.
   */
  public final void addPeriodic(Runnable callback, double period, double offset) {
    m_callbacks.add(
        new Callback(callback, m_startTimeUs, (long) (period * 1e6), (long) (offset * 1e6)));
  }

  /**
   * Add a callback to run at a specific period.
   *
   * <p>This is scheduled on the same Notifier as periodic(), so periodic() and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   */
  public final void addPeriodic(Runnable callback, Time period) {
    addPeriodic(callback, period.in(Seconds));
  }

  /**
   * Add a callback to run at a specific period with a starting time offset.
   *
   * <p>This is scheduled on the same Notifier as periodic(), so periodic() and the callback run
   * synchronously. Interactions between them are thread-safe.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   * @param offset The offset from the common starting time. This is useful for scheduling a
   *     callback in a different timeslot relative to PeriodicOpMode.
   */
  public final void addPeriodic(Runnable callback, Time period, Time offset) {
    addPeriodic(callback, period.in(Seconds), offset.in(Seconds));
  }

  /**
   * Gets time period between calls to Periodic() functions.
   *
   * @return The time period between calls to Periodic() functions.
   */
  public double getPeriod() {
    return m_period;
  }

  /** Loop function. */
  protected void loopFunc() {
    DriverStation.refreshData();
    DriverStation.refreshControlWordFromCache(m_word);
    m_word.setOpModeId(m_opModeId);
    DriverStationJNI.observeUserProgram(m_word.getNative());

    if (!DriverStation.isEnabled() || DriverStation.getOpModeId() != m_opModeId) {
      m_running = false;
      return;
    }

    m_watchdog.reset();
    periodic();
    m_watchdog.addEpoch("OpMode periodic()");

    m_robot.internalRobotPeriodic(m_watchdog);

    m_watchdog.disable();

    // Flush NetworkTables
    NetworkTableInstance.getDefault().flushLocal();

    // Warn on loop time overruns
    if (m_watchdog.isExpired()) {
      m_watchdog.printEpochs();
    }
  }

  // implements OpMode interface
  @Override
  public final void opModeRun(long opModeId) {
    m_opModeId = opModeId;

    start();

    while (m_running) {
      // We don't have to check there's an element in the queue first because
      // there's always at least one (the constructor adds one). It's reenqueued
      // at the end of the loop.
      var callback = m_callbacks.poll();
      NotifierJNI.setNotifierAlarm(m_notifier, callback.expirationTime, 0, true, true);

      try {
        WPIUtilJNI.waitForObject(m_notifier);
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
        break;
      }

      long currentTime = RobotController.getFPGATime();
      m_loopStartTimeUs = RobotController.getFPGATime();

      callback.func.run();

      // Increment the expiration time by the number of full periods it's behind
      // plus one to avoid rapid repeat fires from a large loop overrun. We
      // assume currentTime ≥ expirationTime rather than checking for it since
      // the callback wouldn't be running otherwise.
      callback.expirationTime +=
          callback.period
              + (currentTime - callback.expirationTime) / callback.period * callback.period;
      m_callbacks.add(callback);

      // Process all other callbacks that are ready to run
      while (m_callbacks.peek().expirationTime <= currentTime) {
        callback = m_callbacks.poll();

        callback.func.run();

        callback.expirationTime +=
            callback.period
                + (currentTime - callback.expirationTime) / callback.period * callback.period;
        m_callbacks.add(callback);
      }
    }
    end();
  }

  @Override
  public final void opModeStop() {
    NotifierJNI.destroyNotifier(m_notifier);
    m_notifier = 0;
  }

  @Override
  public final void opModeClose() {
    if (m_notifier != 0) {
      NotifierJNI.destroyNotifier(m_notifier);
    }
    close();
  }

  /** Prints list of epochs added so far and their times. */
  public void printWatchdogEpochs() {
    m_watchdog.printEpochs();
  }

  private void printLoopOverrunMessage() {
    DriverStation.reportWarning("Loop time of " + m_period + "s overrun\n", false);
  }
}
