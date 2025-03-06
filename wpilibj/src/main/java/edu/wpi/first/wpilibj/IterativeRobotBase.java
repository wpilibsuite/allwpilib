// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

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
 * <p>robotInit() -- provide for initialization at robot power-on
 *
 * <p>driverStationConnected() -- provide for initialization the first time the DS is connected
 *
 * <p>init() functions -- each of the following functions is called once when the appropriate mode
 * is entered:
 *
 * <ul>
 *   <li>disabledInit() -- called each and every time disabled is entered from another mode
 *   <li>autonomousInit() -- called each and every time autonomous is entered from another mode
 *   <li>teleopInit() -- called each and every time teleop is entered from another mode
 *   <li>testInit() -- called each and every time test is entered from another mode
 * </ul>
 *
 * <p>periodic() functions -- each of these functions is called on an interval:
 *
 * <ul>
 *   <li>robotPeriodic()
 *   <li>disabledPeriodic()
 *   <li>autonomousPeriodic()
 *   <li>teleopPeriodic()
 *   <li>testPeriodic()
 * </ul>
 *
 * <p>exit() functions -- each of the following functions is called once when the appropriate mode
 * is exited:
 *
 * <ul>
 *   <li>disabledExit() -- called each and every time disabled is exited
 *   <li>autonomousExit() -- called each and every time autonomous is exited
 *   <li>teleopExit() -- called each and every time teleop is exited
 *   <li>testExit() -- called each and every time test is exited
 * </ul>
 */
public abstract class IterativeRobotBase extends RobotBase {
  private final DSControlWord m_word = new DSControlWord();
  private final int m_autonomousMode;
  private final int m_teleoperatedMode;
  private final int m_testMode;
  private int m_lastMode = -1;
  private final double m_period;
  private final Watchdog m_watchdog;
  private boolean m_ntFlushEnabled = true;
  private boolean m_calledDsConnected;

  /**
   * Constructor for IterativeRobotBase.
   *
   * @param period Period in seconds.
   */
  protected IterativeRobotBase(double period) {
    m_autonomousMode = DriverStation.addOpModeOption("auto", "", "", 0);
    m_teleoperatedMode = DriverStation.addOpModeOption("teleop", "", "", 0);
    m_testMode = DriverStation.addOpModeOption("test", "", "", 0);
    m_period = period;
    m_watchdog = new Watchdog(period, this::printLoopOverrunMessage);
  }

  /** Provide an alternate "main loop" via startCompetition(). */
  @Override
  public abstract void startCompetition();

  /* ----------- Overridable initialization code ----------------- */

  /**
   * Robot-wide initialization code should go here.
   *
   * <p>Users should override this method for default Robot-wide initialization which will be called
   * when the robot is first powered on. It will be called exactly one time.
   *
   * <p>Note: This method is functionally identical to the class constructor so that should be used
   * instead.
   */
  public void robotInit() {}

  /**
   * Code that needs to know the DS state should go here.
   *
   * <p>Users should override this method for initialization that needs to occur after the DS is
   * connected, such as needing the alliance information.
   */
  public void driverStationConnected() {}

  /**
   * Robot-wide simulation initialization code should go here.
   *
   * <p>Users should override this method for default Robot-wide simulation related initialization
   * which will be called when the robot is first started. It will be called exactly one time after
   * RobotInit is called only when the robot is in simulation.
   */
  public void simulationInit() {}

  /**
   * Initialization code for disabled mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters disabled mode.
   */
  public void disabledInit() {}

  /**
   * Initialization code for autonomous mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters autonomous mode.
   */
  public void autonomousInit() {}

  /**
   * Initialization code for teleop mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters teleop mode.
   */
  public void teleopInit() {}

  /**
   * Initialization code for test mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters test mode.
   */
  public void testInit() {}

  /* ----------- Overridable periodic code ----------------- */

  private boolean m_rpFirstRun = true;

  /** Periodic code for all robot modes should go here. */
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
   * <p>This function is called in a simulated robot after user code executes.
   */
  public void simulationPeriodic() {
    if (m_spFirstRun) {
      System.out.println("Default simulationPeriodic() method... Override me!");
      m_spFirstRun = false;
    }
  }

  private boolean m_dpFirstRun = true;

  /** Periodic code for disabled mode should go here. */
  public void disabledPeriodic() {
    if (m_dpFirstRun) {
      System.out.println("Default disabledPeriodic() method... Override me!");
      m_dpFirstRun = false;
    }
  }

  private boolean m_apFirstRun = true;

  /** Periodic code for autonomous mode should go here. */
  public void autonomousPeriodic() {
    if (m_apFirstRun) {
      System.out.println("Default autonomousPeriodic() method... Override me!");
      m_apFirstRun = false;
    }
  }

  private boolean m_tpFirstRun = true;

  /** Periodic code for teleop mode should go here. */
  public void teleopPeriodic() {
    if (m_tpFirstRun) {
      System.out.println("Default teleopPeriodic() method... Override me!");
      m_tpFirstRun = false;
    }
  }

  private boolean m_tmpFirstRun = true;

  /** Periodic code for test mode should go here. */
  public void testPeriodic() {
    if (m_tmpFirstRun) {
      System.out.println("Default testPeriodic() method... Override me!");
      m_tmpFirstRun = false;
    }
  }

  /**
   * Exit code for disabled mode should go here.
   *
   * <p>Users should override this method for code which will be called each time the robot exits
   * disabled mode.
   */
  public void disabledExit() {}

  /**
   * Exit code for autonomous mode should go here.
   *
   * <p>Users should override this method for code which will be called each time the robot exits
   * autonomous mode.
   */
  public void autonomousExit() {}

  /**
   * Exit code for teleop mode should go here.
   *
   * <p>Users should override this method for code which will be called each time the robot exits
   * teleop mode.
   */
  public void teleopExit() {}

  /**
   * Exit code for test mode should go here.
   *
   * <p>Users should override this method for code which will be called each time the robot exits
   * test mode.
   */
  public void testExit() {}

  /**
   * Returns true if in autonomous mode.
   *
   * @return True if autonomous
   */
  public boolean isAutonomous() {
    return DriverStation.getOpModeId() == m_autonomousMode;
  }

  /**
   * Returns true if in teleoperated mode.
   *
   * @return True if teleoperated
   */
  public boolean isTeleoperated() {
    return DriverStation.getOpModeId() == m_teleoperatedMode;
  }

  /**
   * Returns true if in test mode.
   *
   * @return True if test
   */
  public boolean isTest() {
    return DriverStation.getOpModeId() == m_testMode;
  }

  /**
   * Enables or disables flushing NetworkTables every loop iteration. By default, this is enabled.
   *
   * @param enabled True to enable, false to disable
   * @deprecated Deprecated without replacement.
   */
  @Deprecated(forRemoval = true, since = "2025")
  public void setNetworkTablesFlushEnabled(boolean enabled) {
    m_ntFlushEnabled = enabled;
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
    m_watchdog.reset();

    m_word.refresh();

    // Get current mode
    int mode = DriverStation.getOpModeId();

    if (!m_calledDsConnected && m_word.isDSAttached()) {
      m_calledDsConnected = true;
      driverStationConnected();
    }

    // If mode changed, call mode exit and entry functions
    if (m_lastMode != mode) {
      // Call last mode's exit function
      if (m_lastMode == 0) {
        disabledExit();
      } else if (m_lastMode == m_autonomousMode) {
        autonomousExit();
      } else if (m_lastMode == m_teleoperatedMode) {
        teleopExit();
      } else if (m_lastMode == m_testMode) {
        testExit();
      }

      // Call current mode's entry function
      if (mode == 0) {
        disabledInit();
        m_watchdog.addEpoch("disabledInit()");
      } else if (mode == m_autonomousMode) {
        autonomousInit();
        m_watchdog.addEpoch("autonomousInit()");
      } else if (mode == m_teleoperatedMode) {
        teleopInit();
        m_watchdog.addEpoch("teleopInit()");
      } else if (mode == m_testMode) {
        testInit();
        m_watchdog.addEpoch("testInit()");
      }

      m_lastMode = mode;
    }

    // Call the appropriate function depending upon the current robot mode
    DriverStationJNI.observeUserProgramOpMode(mode);
    if (mode == 0) {
      disabledPeriodic();
      m_watchdog.addEpoch("disabledPeriodic()");
    } else if (mode == m_autonomousMode) {
      autonomousPeriodic();
      m_watchdog.addEpoch("autonomousPeriodic()");
    } else if (mode == m_teleoperatedMode) {
      teleopPeriodic();
      m_watchdog.addEpoch("teleopPeriodic()");
    } else if (mode == m_testMode) {
      testPeriodic();
      m_watchdog.addEpoch("testPeriodic()");
    }

    robotPeriodic();
    m_watchdog.addEpoch("robotPeriodic()");

    SmartDashboard.updateValues();
    m_watchdog.addEpoch("SmartDashboard.updateValues()");

    if (isSimulation()) {
      HAL.simPeriodicBefore();
      simulationPeriodic();
      HAL.simPeriodicAfter();
      m_watchdog.addEpoch("simulationPeriodic()");
    }

    m_watchdog.disable();

    // Flush NetworkTables
    if (m_ntFlushEnabled) {
      NetworkTableInstance.getDefault().flushLocal();
    }

    // Warn on loop time overruns
    if (m_watchdog.isExpired()) {
      m_watchdog.printEpochs();
    }
  }

  /** Prints list of epochs added so far and their times. */
  public void printWatchdogEpochs() {
    m_watchdog.printEpochs();
  }

  private void printLoopOverrunMessage() {
    DriverStation.reportWarning("Loop time of " + m_period + "s overrun\n", false);
  }
}
