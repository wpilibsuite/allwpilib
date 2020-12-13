/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
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
 * <p>init() functions -- each of the following functions is called once when the
 * appropriate mode is entered:
 *   - disabledInit()   -- called each and every time disabled is entered from
 *                         another mode
 *   - autonomousInit() -- called each and every time autonomous is entered from
 *                         another mode
 *   - teleopInit()     -- called each and every time teleop is entered from
 *                         another mode
 *   - testInit()       -- called each and every time test is entered from
 *                         another mode
 *
 * <p>periodic() functions -- each of these functions is called on an interval:
 *   - robotPeriodic()
 *   - disabledPeriodic()
 *   - autonomousPeriodic()
 *   - teleopPeriodic()
 *   - testPeriodic()
 */
public abstract class IterativeRobotBase extends RobotBase {
  protected double m_period;

  private enum Mode {
    kNone,
    kDisabled,
    kAutonomous,
    kTeleop,
    kTest
  }

  private Mode m_lastMode = Mode.kNone;
  private final Watchdog m_watchdog;
  private boolean m_ntFlushEnabled;

  /**
   * Constructor for IterativeRobotBase.
   *
   * @param period Period in seconds.
   */
  protected IterativeRobotBase(double period) {
    m_period = period;
    m_watchdog = new Watchdog(period, this::printLoopOverrunMessage);
  }

  /**
   * Provide an alternate "main loop" via startCompetition().
   */
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
  public void robotInit() {
    System.out.println("Default robotInit() method... Override me!");
  }

  /**
   * Robot-wide simulation initialization code should go here.
   *
   * <p>Users should override this method for default Robot-wide simulation
   * related initialization which will be called when the robot is first
   * started. It will be called exactly one time after RobotInit is called
   * only when the robot is in simulation.
   */
  public void simulationInit() {
    System.out.println("Default simulationInit() method... Override me!");
  }

  /**
   * Initialization code for disabled mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters disabled mode.
   */
  public void disabledInit() {
    System.out.println("Default disabledInit() method... Override me!");
  }

  /**
   * Initialization code for autonomous mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters autonomous mode.
   */
  public void autonomousInit() {
    System.out.println("Default autonomousInit() method... Override me!");
  }

  /**
   * Initialization code for teleop mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters teleop mode.
   */
  public void teleopInit() {
    System.out.println("Default teleopInit() method... Override me!");
  }

  /**
   * Initialization code for test mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters test mode.
   */
  @SuppressWarnings("PMD.JUnit4TestShouldUseTestAnnotation")
  public void testInit() {
    System.out.println("Default testInit() method... Override me!");
  }

  /* ----------- Overridable periodic code ----------------- */

  private boolean m_rpFirstRun = true;

  /**
   * Periodic code for all robot modes should go here.
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
   * <p>This function is called in a simulated robot after user code executes.
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
   */
  public void autonomousPeriodic() {
    if (m_apFirstRun) {
      System.out.println("Default autonomousPeriodic() method... Override me!");
      m_apFirstRun = false;
    }
  }

  private boolean m_tpFirstRun = true;

  /**
   * Periodic code for teleop mode should go here.
   */
  public void teleopPeriodic() {
    if (m_tpFirstRun) {
      System.out.println("Default teleopPeriodic() method... Override me!");
      m_tpFirstRun = false;
    }
  }

  private boolean m_tmpFirstRun = true;

  /**
   * Periodic code for test mode should go here.
   */
  @SuppressWarnings("PMD.JUnit4TestShouldUseTestAnnotation")
  public void testPeriodic() {
    if (m_tmpFirstRun) {
      System.out.println("Default testPeriodic() method... Override me!");
      m_tmpFirstRun = false;
    }
  }

  /**
   * Enables or disables flushing NetworkTables every loop iteration.
   * By default, this is disabled.
   *
   * @param enabled True to enable, false to disable
   */
  public void setNetworkTablesFlushEnabled(boolean enabled) {
    m_ntFlushEnabled = enabled;
  }

  @SuppressWarnings("PMD.CyclomaticComplexity")
  protected void loopFunc() {
    m_watchdog.reset();

    // Call the appropriate function depending upon the current robot mode
    if (isDisabled()) {
      // Call DisabledInit() if we are now just entering disabled mode from either a different mode
      // or from power-on.
      if (m_lastMode != Mode.kDisabled) {
        LiveWindow.setEnabled(false);
        Shuffleboard.disableActuatorWidgets();
        disabledInit();
        m_watchdog.addEpoch("disabledInit()");
        m_lastMode = Mode.kDisabled;
      }

      HAL.observeUserProgramDisabled();
      disabledPeriodic();
      m_watchdog.addEpoch("disablePeriodic()");
    } else if (isAutonomous()) {
      // Call AutonomousInit() if we are now just entering autonomous mode from either a different
      // mode or from power-on.
      if (m_lastMode != Mode.kAutonomous) {
        LiveWindow.setEnabled(false);
        Shuffleboard.disableActuatorWidgets();
        autonomousInit();
        m_watchdog.addEpoch("autonomousInit()");
        m_lastMode = Mode.kAutonomous;
      }

      HAL.observeUserProgramAutonomous();
      autonomousPeriodic();
      m_watchdog.addEpoch("autonomousPeriodic()");
    } else if (isOperatorControl()) {
      // Call TeleopInit() if we are now just entering teleop mode from either a different mode or
      // from power-on.
      if (m_lastMode != Mode.kTeleop) {
        LiveWindow.setEnabled(false);
        Shuffleboard.disableActuatorWidgets();
        teleopInit();
        m_watchdog.addEpoch("teleopInit()");
        m_lastMode = Mode.kTeleop;
      }

      HAL.observeUserProgramTeleop();
      teleopPeriodic();
      m_watchdog.addEpoch("teleopPeriodic()");
    } else {
      // Call TestInit() if we are now just entering test mode from either a different mode or from
      // power-on.
      if (m_lastMode != Mode.kTest) {
        LiveWindow.setEnabled(true);
        Shuffleboard.enableActuatorWidgets();
        testInit();
        m_watchdog.addEpoch("testInit()");
        m_lastMode = Mode.kTest;
      }

      HAL.observeUserProgramTest();
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
      NetworkTableInstance.getDefault().flush();
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
