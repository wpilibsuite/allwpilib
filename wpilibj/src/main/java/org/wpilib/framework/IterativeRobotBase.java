// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.system.Watchdog;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.util.Alert;

/**
 * IterativeRobotBase implements a specific type of robot program framework, extending the RobotBase
 * class.
 *
 * <p>The IterativeRobotBase class does not implement startCompetition(), so it should not be used
 * by teams directly.
 *
 * <p>This class provides the following functions which are called by the main loop,
 * startCompetition(), at the appropriate times:
 *
 * <p>driverStationConnected() -- provide for initialization the first time the DS is connected
 *
 * <p>start() functions -- each of the following functions is called once when the appropriate mode
 * starts:
 *
 * <ul>
 *   <li>disabledStart() -- called each and every time disabled mode starts
 *   <li>autonomousStart() -- called each and every time autonomous mode starts
 *   <li>teleopStart() -- called each and every time teleoperated mode starts
 *   <li>utilityStart() -- called each and every time utility mode starts
 * </ul>
 *
 * <p>periodic() functions -- each of these functions is called on an interval:
 *
 * <ul>
 *   <li>robotPeriodic()
 *   <li>disabledPeriodic()
 *   <li>autonomousPeriodic()
 *   <li>teleopPeriodic()
 *   <li>utilityPeriodic()
 * </ul>
 *
 * <p>Order within one loop iteration:
 *
 * <ol>
 *   <li>{@link #driverStationConnected()}, once, the first time the DS is connected
 *   <li>the end function of the previous mode and the start function of the new mode, only on a
 *       mode change
 *   <li>the periodic function for the current mode ({@link #disabledPeriodic()}, {@link
 *       #autonomousPeriodic()}, {@link #teleopPeriodic()}, or {@link #utilityPeriodic()})
 *   <li>{@link #robotPeriodic()}
 *   <li>{@link #simulationPeriodic()}, only in simulation
 * </ol>
 *
 * <p>The mode-specific periodic function runs before {@link #robotPeriodic()}. Exactly one
 * mode-specific periodic function runs per iteration.
 *
 * <p>end() functions -- each of the following functions is called once when the appropriate mode is
 * ends:
 *
 * <ul>
 *   <li>disabledEnd() -- called each and every time disabled mode ends
 *   <li>autonomousEnd() -- called each and every time autonomous mode ends
 *   <li>teleopEnd() -- called each and every time teleoperated mode ends
 *   <li>utilityEnd() -- called each and every time utility mode ends
 * </ul>
 */
public abstract class IterativeRobotBase extends RobotBase {
  private final ControlWord m_word = new ControlWord();
  private RobotMode m_lastMode;
  private final double m_period;
  private final Watchdog m_watchdog;
  private final Alert m_loopOverrunAlert;
  private boolean m_calledDsConnected;

  /**
   * Constructor for IterativeRobotBase.
   *
   * @param period Period in seconds.
   */
  protected IterativeRobotBase(double period) {
    m_period = period;
    m_loopOverrunAlert =
        new Alert(
            "iterativerobot-loop-overrun",
            "Loop time of " + m_period + "s overrun",
            Alert.Level.MEDIUM);
    m_watchdog = new Watchdog(period, () -> m_loopOverrunAlert.set(true));
  }

  /** Provide an alternate "main loop" via startCompetition(). */
  @Override
  public abstract void startCompetition();

  /* ----------- Overridable initialization code ----------------- */

  /**
   * Code that needs to know the DS state should go here.
   *
   * <p>Users should override this method for initialization that needs to occur after the DS is
   * connected, such as needing the alliance information.
   */
  public void driverStationConnected() {}

  /**
   * Start code for simulation should go here.
   *
   * <p>Users should override this method for default Robot-wide simulation related initialization
   * which will be called when the robot is first started. It will be called exactly one time after
   * the robot class constructor is called only when the robot is in simulation.
   */
  public void simulationStart() {}

  /**
   * Start code for disabled mode should go here.
   *
   * <p>Users should override this method for code which will be called each time disabled mode
   * starts.
   */
  public void disabledStart() {}

  /**
   * Start code for autonomous mode should go here.
   *
   * <p>Users should override this method for code which will be called each time autonomous mode
   * starts.
   */
  public void autonomousStart() {}

  /**
   * Start code for teleoperated mode should go here.
   *
   * <p>Users should override this method for code which will be called each time teleoperated mode
   * starts.
   */
  public void teleopStart() {}

  /**
   * Start code for utility mode should go here.
   *
   * <p>Users should override this method for code which will be called each time utility mode
   * starts.
   */
  public void utilityStart() {}

  /* ----------- Overridable periodic code ----------------- */

  private boolean m_rpFirstRun = true;

  /**
   * Periodic code for all robot modes should go here.
   *
   * <p>Runs after the current mode's periodic function ({@link #disabledPeriodic()}, {@link
   * #autonomousPeriodic()}, {@link #teleopPeriodic()}, or {@link #utilityPeriodic()}) and before
   * {@link #simulationPeriodic()}.
   */
  public void robotPeriodic() {
    if (m_rpFirstRun) {
      System.out.println("Default robotPeriodic() method... Override me!");
      m_rpFirstRun = false;
    }
  }

  private boolean m_spFirstRun = true;

  /**
   * Periodic simulation code should go here.
   *
   * <p>This function is called in a simulated robot after user code executes. Within a loop
   * iteration it runs last, after the current mode's periodic function and {@link
   * #robotPeriodic()}.
   */
  public void simulationPeriodic() {
    if (m_spFirstRun) {
      System.out.println("Default simulationPeriodic() method... Override me!");
      m_spFirstRun = false;
    }
  }

  private boolean m_dpFirstRun = true;

  /**
   * Periodic code for disabled mode should go here.
   *
   * <p>Runs before {@link #robotPeriodic()} and {@link #simulationPeriodic()}. Never runs in the
   * same iteration as another mode's periodic function.
   */
  public void disabledPeriodic() {
    if (m_dpFirstRun) {
      System.out.println("Default disabledPeriodic() method... Override me!");
      m_dpFirstRun = false;
    }
  }

  private boolean m_apFirstRun = true;

  /**
   * Periodic code for autonomous mode should go here.
   *
   * <p>Runs before {@link #robotPeriodic()} and {@link #simulationPeriodic()}.
   */
  public void autonomousPeriodic() {
    if (m_apFirstRun) {
      System.out.println("Default autonomousPeriodic() method... Override me!");
      m_apFirstRun = false;
    }
  }

  private boolean m_tpFirstRun = true;

  /**
   * Periodic code for teleoperated mode should go here.
   *
   * <p>Runs before {@link #robotPeriodic()} and {@link #simulationPeriodic()}.
   */
  public void teleopPeriodic() {
    if (m_tpFirstRun) {
      System.out.println("Default teleopPeriodic() method... Override me!");
      m_tpFirstRun = false;
    }
  }

  private boolean m_tmpFirstRun = true;

  /**
   * Periodic code for utility mode should go here.
   *
   * <p>Runs before {@link #robotPeriodic()} and {@link #simulationPeriodic()}.
   */
  public void utilityPeriodic() {
    if (m_tmpFirstRun) {
      System.out.println("Default utilityPeriodic() method... Override me!");
      m_tmpFirstRun = false;
    }
  }

  /**
   * End code for disabled mode should go here.
   *
   * <p>Users should override this method for code which will be called each time disabled mode
   * ends.
   */
  public void disabledEnd() {}

  /**
   * End code for autonomous mode should go here.
   *
   * <p>Users should override this method for code which will be called each time autonomous mode
   * ends.
   */
  public void autonomousEnd() {}

  /**
   * End code for teleoperated mode should go here.
   *
   * <p>Users should override this method for code which will be called each time teleoperated mode
   * ends.
   */
  public void teleopEnd() {}

  /**
   * End code for utility mode should go here.
   *
   * <p>Users should override this method for code which will be called each time utility mode ends.
   */
  public void utilityEnd() {}

  /**
   * Gets time period between calls to Periodic() functions.
   *
   * @return The time period between calls to Periodic() functions.
   */
  public double getPeriod() {
    return m_period;
  }

  /** Loop function. */
  protected final void loopFunc() {
    DriverStationBackend.refreshData();
    DriverStationBackend.refreshControlWordFromCache(m_word);
    m_watchdog.reset();

    // Get current mode; treat disabled as unknown
    boolean enabled = m_word.isEnabled();
    RobotMode mode = enabled ? m_word.getRobotMode() : RobotMode.UNKNOWN;

    if (!m_calledDsConnected && m_word.isDSAttached()) {
      m_calledDsConnected = true;
      driverStationConnected();
    }

    // If mode changed, call mode end and start functions
    if (m_lastMode != mode) {
      if (m_lastMode != null) {
        // Call last mode's end function
        switch (m_lastMode) {
          case UNKNOWN -> disabledEnd();
          case AUTONOMOUS -> autonomousEnd();
          case TELEOPERATED -> teleopEnd();
          case UTILITY -> utilityEnd();
          default -> {
            // NOP
          }
        }
      }

      // Call current mode's start function
      switch (mode) {
        case UNKNOWN -> {
          disabledStart();
          m_watchdog.addEpoch("disabledStart()");
        }
        case AUTONOMOUS -> {
          autonomousStart();
          m_watchdog.addEpoch("autonomousStart()");
        }
        case TELEOPERATED -> {
          teleopStart();
          m_watchdog.addEpoch("teleopStart()");
        }
        case UTILITY -> {
          utilityStart();
          m_watchdog.addEpoch("utilityStart()");
        }
        default -> {
          // NOP
        }
      }

      m_lastMode = mode;
    }

    // Call the appropriate function depending upon the current robot mode
    DriverStationJNI.observeUserProgram(m_word.getNative());
    switch (mode) {
      case UNKNOWN -> {
        disabledPeriodic();
        m_watchdog.addEpoch("disabledPeriodic()");
      }
      case AUTONOMOUS -> {
        autonomousPeriodic();
        m_watchdog.addEpoch("autonomousPeriodic()");
      }
      case TELEOPERATED -> {
        teleopPeriodic();
        m_watchdog.addEpoch("teleopPeriodic()");
      }
      case UTILITY -> {
        utilityPeriodic();
        m_watchdog.addEpoch("utilityPeriodic()");
      }
      default -> {
        // NOP
      }
    }

    robotPeriodic();
    m_watchdog.addEpoch("robotPeriodic()");

    TunableRegistry.update();
    m_watchdog.addEpoch("TunableRegistry.update()");

    if (isSimulation()) {
      HAL.simPeriodicBefore();
      simulationPeriodic();
      HAL.simPeriodicAfter();
      m_watchdog.addEpoch("simulationPeriodic()");
    }

    m_watchdog.disable();

    // Flush NetworkTables
    NetworkTableInstance.getDefault().flushLocal();

    // Warn on loop time overruns, and clear the alert once the loop is back on time
    if (m_watchdog.isExpired()) {
      m_watchdog.printEpochs();
    }
    m_loopOverrunAlert.set(m_watchdog.isExpired());
  }

  /** Prints list of epochs added so far and their times. */
  public void printWatchdogEpochs() {
    m_watchdog.printEpochs();
  }

  @Override
  public void close() {
    m_loopOverrunAlert.close();
    super.close();
  }
}
