/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
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
 *   - disabledInit()   -- called only when first disabled
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
  private enum Mode {
    kNone,
    kDisabled,
    kAutonomous,
    kTeleop,
    kTest
  }

  private Mode m_lastMode = Mode.kNone;

  /**
   * Provide an alternate "main loop" via startCompetition().
   */
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
    System.out.println("Default robotInit() method... Overload me!");
  }

  /**
   * Initialization code for disabled mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters disabled mode.
   */
  public void disabledInit() {
    System.out.println("Default disabledInit() method... Overload me!");
  }

  /**
   * Initialization code for autonomous mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters autonomous mode.
   */
  public void autonomousInit() {
    System.out.println("Default autonomousInit() method... Overload me!");
  }

  /**
   * Initialization code for teleop mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters teleop mode.
   */
  public void teleopInit() {
    System.out.println("Default teleopInit() method... Overload me!");
  }

  /**
   * Initialization code for test mode should go here.
   *
   * <p>Users should override this method for initialization code which will be called each time the
   * robot enters test mode.
   */
  @SuppressWarnings("PMD.JUnit4TestShouldUseTestAnnotation")
  public void testInit() {
    System.out.println("Default testInit() method... Overload me!");
  }

  /* ----------- Overridable periodic code ----------------- */

  private boolean m_rpFirstRun = true;

  /**
   * Periodic code for all robot modes should go here.
   */
  public void robotPeriodic() {
    if (m_rpFirstRun) {
      System.out.println("Default robotPeriodic() method... Overload me!");
      m_rpFirstRun = false;
    }
  }

  private boolean m_dpFirstRun = true;

  /**
   * Periodic code for disabled mode should go here.
   */
  public void disabledPeriodic() {
    if (m_dpFirstRun) {
      System.out.println("Default disabledPeriodic() method... Overload me!");
      m_dpFirstRun = false;
    }
  }

  private boolean m_apFirstRun = true;

  /**
   * Periodic code for autonomous mode should go here.
   */
  public void autonomousPeriodic() {
    if (m_apFirstRun) {
      System.out.println("Default autonomousPeriodic() method... Overload me!");
      m_apFirstRun = false;
    }
  }

  private boolean m_tpFirstRun = true;

  /**
   * Periodic code for teleop mode should go here.
   */
  public void teleopPeriodic() {
    if (m_tpFirstRun) {
      System.out.println("Default teleopPeriodic() method... Overload me!");
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
      System.out.println("Default testPeriodic() method... Overload me!");
      m_tmpFirstRun = false;
    }
  }

  protected void loopFunc() {
    // Call the appropriate function depending upon the current robot mode
    if (isDisabled()) {
      // call DisabledInit() if we are now just entering disabled mode from
      // either a different mode or from power-on
      if (m_lastMode != Mode.kDisabled) {
        LiveWindow.setEnabled(false);
        disabledInit();
        m_lastMode = Mode.kDisabled;
      }
      HAL.observeUserProgramDisabled();
      disabledPeriodic();
    } else if (isAutonomous()) {
      // call Autonomous_Init() if this is the first time
      // we've entered autonomous_mode
      if (m_lastMode != Mode.kAutonomous) {
        LiveWindow.setEnabled(false);
        // KBS NOTE: old code reset all PWMs and relays to "safe values"
        // whenever entering autonomous mode, before calling
        // "Autonomous_Init()"
        autonomousInit();
        m_lastMode = Mode.kAutonomous;
      }
      HAL.observeUserProgramAutonomous();
      autonomousPeriodic();
    } else if (isOperatorControl()) {
      // call Teleop_Init() if this is the first time
      // we've entered teleop_mode
      if (m_lastMode != Mode.kTeleop) {
        LiveWindow.setEnabled(false);
        teleopInit();
        m_lastMode = Mode.kTeleop;
      }
      HAL.observeUserProgramTeleop();
      teleopPeriodic();
    } else {
      // call TestInit() if we are now just entering test mode from either
      // a different mode or from power-on
      if (m_lastMode != Mode.kTest) {
        LiveWindow.setEnabled(true);
        testInit();
        m_lastMode = Mode.kTest;
      }
      HAL.observeUserProgramTest();
      testPeriodic();
    }
    robotPeriodic();
    SmartDashboard.updateValues();
    LiveWindow.updateValues();
  }
}
