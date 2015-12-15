/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "IterativeRobot.h"

#include "DriverStation.h"
#include "HAL/HAL.hpp"
#include "SmartDashboard/SmartDashboard.h"
#include "LiveWindow/LiveWindow.h"
#include "networktables/NetworkTable.h"

constexpr double IterativeRobot::kDefaultPeriod;

/**
 * Provide an alternate "main loop" via StartCompetition().
 *
 * This specific StartCompetition() implements "main loop" behaviour synced with
 * the DS packets
 */
void IterativeRobot::StartCompetition() {
  HALReport(HALUsageReporting::kResourceType_Framework,
            HALUsageReporting::kFramework_Iterative);

  LiveWindow *lw = LiveWindow::GetInstance();
  // first and one-time initialization
  SmartDashboard::init();
  NetworkTable::GetTable("LiveWindow")
      ->GetSubTable("~STATUS~")
      ->PutBoolean("LW Enabled", false);
  RobotInit();

  // Tell the DS that the robot is ready to be enabled
  HALNetworkCommunicationObserveUserProgramStarting();

  // loop forever, calling the appropriate mode-dependent function
  lw->SetEnabled(false);
  while (true) {
    // Call the appropriate function depending upon the current robot mode
    if (IsDisabled()) {
      // call DisabledInit() if we are now just entering disabled mode from
      // either a different mode or from power-on
      if (!m_disabledInitialized) {
        lw->SetEnabled(false);
        DisabledInit();
        m_disabledInitialized = true;
        // reset the initialization flags for the other modes
        m_autonomousInitialized = false;
        m_teleopInitialized = false;
        m_testInitialized = false;
      }
      HALNetworkCommunicationObserveUserProgramDisabled();
      DisabledPeriodic();
    } else if (IsAutonomous()) {
      // call AutonomousInit() if we are now just entering autonomous mode from
      // either a different mode or from power-on
      if (!m_autonomousInitialized) {
        lw->SetEnabled(false);
        AutonomousInit();
        m_autonomousInitialized = true;
        // reset the initialization flags for the other modes
        m_disabledInitialized = false;
        m_teleopInitialized = false;
        m_testInitialized = false;
      }
      HALNetworkCommunicationObserveUserProgramAutonomous();
      AutonomousPeriodic();
    } else if (IsTest()) {
      // call TestInit() if we are now just entering test mode from
      // either a different mode or from power-on
      if (!m_testInitialized) {
        lw->SetEnabled(true);
        TestInit();
        m_testInitialized = true;
        // reset the initialization flags for the other modes
        m_disabledInitialized = false;
        m_autonomousInitialized = false;
        m_teleopInitialized = false;
      }
      HALNetworkCommunicationObserveUserProgramTest();
      TestPeriodic();
    } else {
      // call TeleopInit() if we are now just entering teleop mode from
      // either a different mode or from power-on
      if (!m_teleopInitialized) {
        lw->SetEnabled(false);
        TeleopInit();
        m_teleopInitialized = true;
        // reset the initialization flags for the other modes
        m_disabledInitialized = false;
        m_autonomousInitialized = false;
        m_testInitialized = false;
        Scheduler::GetInstance()->SetEnabled(true);
      }
      HALNetworkCommunicationObserveUserProgramTeleop();
      TeleopPeriodic();
    }
    // wait for driver station data so the loop doesn't hog the CPU
    m_ds.WaitForData();
  }
}

/**
 * Robot-wide initialization code should go here.
 *
 * Users should override this method for default Robot-wide initialization which
 * will be called when the robot is first powered on. It will be called exactly
 * one time.
 *
 * Warning: the Driver Station "Robot Code" light and FMS "Robot Ready"
 * indicators will be off until RobotInit() exits. Code in RobotInit() that
 * waits for enable will cause the robot to never indicate that the code is
 * ready, causing the robot to be bypassed in a match.
 */
void IterativeRobot::RobotInit() {
  printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Initialization code for disabled mode should go here.
 *
 * Users should override this method for initialization code which will be
 * called each time
 * the robot enters disabled mode.
 */
void IterativeRobot::DisabledInit() {
  printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Initialization code for autonomous mode should go here.
 *
 * Users should override this method for initialization code which will be
 * called each time
 * the robot enters autonomous mode.
 */
void IterativeRobot::AutonomousInit() {
  printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Initialization code for teleop mode should go here.
 *
 * Users should override this method for initialization code which will be
 * called each time
 * the robot enters teleop mode.
 */
void IterativeRobot::TeleopInit() {
  printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Initialization code for test mode should go here.
 *
 * Users should override this method for initialization code which will be
 * called each time
 * the robot enters test mode.
 */
void IterativeRobot::TestInit() {
  printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Periodic code for disabled mode should go here.
 *
 * Users should override this method for code which will be called periodically
 * at a regular
 * rate while the robot is in disabled mode.
 */
void IterativeRobot::DisabledPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    printf("Default %s() method... Overload me!\n", __FUNCTION__);
    firstRun = false;
  }
  delayTicks(1);
}

/**
 * Periodic code for autonomous mode should go here.
 *
 * Users should override this method for code which will be called periodically
 * at a regular
 * rate while the robot is in autonomous mode.
 */
void IterativeRobot::AutonomousPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    printf("Default %s() method... Overload me!\n", __FUNCTION__);
    firstRun = false;
  }
  delayTicks(1);
}

/**
 * Periodic code for teleop mode should go here.
 *
 * Users should override this method for code which will be called periodically
 * at a regular
 * rate while the robot is in teleop mode.
 */
void IterativeRobot::TeleopPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    printf("Default %s() method... Overload me!\n", __FUNCTION__);
    firstRun = false;
  }
  delayTicks(1);
}

/**
 * Periodic code for test mode should go here.
 *
 * Users should override this method for code which will be called periodically
 * at a regular
 * rate while the robot is in test mode.
 */
void IterativeRobot::TestPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    printf("Default %s() method... Overload me!\n", __FUNCTION__);
    firstRun = false;
  }
  delayTicks(1);
}
