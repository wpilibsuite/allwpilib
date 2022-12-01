// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import java.util.ConcurrentModificationException;

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
  private enum Mode {
    kNone,
    kDisabled,
    kAutonomous,
    kTeleop,
    kTest
  }

  private final DSControlWord m_word = new DSControlWord();
  private Mode m_lastMode = Mode.kNone;
  private final double m_period;
  private final Watchdog m_watchdog;
  private boolean m_ntFlushEnabled = true;
  private boolean m_lwEnabledInTest = true;

  /**
   * Constructor for IterativeRobotBase.
   *
   * @param period Period in seconds.
   */
  protected IterativeRobotBase(double period) {
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
   * <p>Warning: the Driver Station "Robot Code" light and FMS "Robot Ready" indicators will be off
   * until RobotInit() exits. Code in RobotInit() that waits for enable will cause the robot to
   * never indicate that the code is ready, causing the robot to be bypassed in a match.
   */
  public void robotInit() {}

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
   * Enables or disables flushing NetworkTables every loop iteration. By default, this is enabled.
   *
   * @param enabled True to enable, false to disable
   */
  public void setNetworkTablesFlushEnabled(boolean enabled) {
    m_ntFlushEnabled = enabled;
  }

  /**
   * Sets whether LiveWindow operation is enabled during test mode. Calling
   *
   * @param testLW True to enable, false to disable. Defaults to true.
   * @throws ConcurrentModificationException if this is called during test mode.
   */
  public void enableLiveWindowInTest(boolean testLW) {
    if (isTest()) {
      throw new ConcurrentModificationException("Can't configure test mode while in test mode!");
    }
    m_lwEnabledInTest = testLW;
  }

  /**
   * Whether LiveWindow operation is enabled during test mode.
   *
   * @return whether LiveWindow should be enabled in test mode.
   */
  public boolean isLiveWindowEnabledInTest() {
    return m_lwEnabledInTest;
  }

  /**
   * Gets time period between calls to Periodic() functions.
   *
   * @return The time period between calls to Periodic() functions.
   */
  public double getPeriod() {
    return m_period;
  }

  protected void loopFunc() {
    DriverStation.refreshData();
    m_watchdog.reset();

    m_word.refresh();

    // Get current mode
    Mode mode = Mode.kNone;
    if (m_word.isDisabled()) {
      mode = Mode.kDisabled;
    } else if (m_word.isAutonomous()) {
      mode = Mode.kAutonomous;
    } else if (m_word.isTeleop()) {
      mode = Mode.kTeleop;
    } else if (m_word.isTest()) {
      mode = Mode.kTest;
    }

    // If mode changed, call mode exit and entry functions
    if (m_lastMode != mode) {
      // Call last mode's exit function
      if (m_lastMode == Mode.kDisabled) {
        disabledExit();
      } else if (m_lastMode == Mode.kAutonomous) {
        autonomousExit();
      } else if (m_lastMode == Mode.kTeleop) {
        teleopExit();
      } else if (m_lastMode == Mode.kTest) {
        if (m_lwEnabledInTest) {
          LiveWindow.setEnabled(false);
          Shuffleboard.disableActuatorWidgets();
        }
        testExit();
      }

      // Call current mode's entry function
      if (mode == Mode.kDisabled) {
        disabledInit();
        m_watchdog.addEpoch("disabledInit()");
      } else if (mode == Mode.kAutonomous) {
        autonomousInit();
        m_watchdog.addEpoch("autonomousInit()");
      } else if (mode == Mode.kTeleop) {
        teleopInit();
        m_watchdog.addEpoch("teleopInit()");
      } else if (mode == Mode.kTest) {
        if (m_lwEnabledInTest) {
          LiveWindow.setEnabled(true);
          Shuffleboard.enableActuatorWidgets();
        }
        testInit();
        m_watchdog.addEpoch("testInit()");
      }

      m_lastMode = mode;
    }

    // Call the appropriate function depending upon the current robot mode
    if (mode == Mode.kDisabled) {
      DriverStationJNI.observeUserProgramDisabled();
      disabledPeriodic();
      m_watchdog.addEpoch("disabledPeriodic()");
    } else if (mode == Mode.kAutonomous) {
      DriverStationJNI.observeUserProgramAutonomous();
      autonomousPeriodic();
      m_watchdog.addEpoch("autonomousPeriodic()");
    } else if (mode == Mode.kTeleop) {
      DriverStationJNI.observeUserProgramTeleop();
      teleopPeriodic();
      m_watchdog.addEpoch("teleopPeriodic()");
    } else {
      DriverStationJNI.observeUserProgramTest();
      testPeriodic();
      m_watchdog.addEpoch("testPeriodic()");
    }

    robotPeriodic();
    m_watchdog.addEpoch("robotPeriodic()");

    SmartDashboard.updateValues();
    m_watchdog.addEpoch("SmartDashboard.updateValues()");
    LiveWindow.updateValues();
    m_watchdog.addEpoch("LiveWindow.updateValues()");
    Shuffleboard.update();
    m_watchdog.addEpoch("Shuffleboard.update()");

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

  private void printLoopOverrunMessage() {
    DriverStation.reportWarning("Loop time of " + m_period + "s overrun\n", false);
  }
}
