/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "IterativeRobotBase.h"

#include <cstdio>

#include <HAL/HAL.h>
#include <llvm/raw_ostream.h>

#include "Commands/Scheduler.h"
#include "LiveWindow/LiveWindow.h"
#include "SmartDashboard/SmartDashboard.h"

using namespace frc;

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
void IterativeRobotBase::RobotInit() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Initialization code for disabled mode should go here.
 *
 * Users should override this method for initialization code which will be
 * called each time
 * the robot enters disabled mode.
 */
void IterativeRobotBase::DisabledInit() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Initialization code for autonomous mode should go here.
 *
 * Users should override this method for initialization code which will be
 * called each time the robot enters autonomous mode.
 */
void IterativeRobotBase::AutonomousInit() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Initialization code for teleop mode should go here.
 *
 * Users should override this method for initialization code which will be
 * called each time the robot enters teleop mode.
 */
void IterativeRobotBase::TeleopInit() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Initialization code for test mode should go here.
 *
 * Users should override this method for initialization code which will be
 * called each time the robot enters test mode.
 */
void IterativeRobotBase::TestInit() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Periodic code for all modes should go here.
 *
 * This function is called each time a new packet is received from the driver
 * station.
 */
void IterativeRobotBase::RobotPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

/**
 * Periodic code for disabled mode should go here.
 *
 * Users should override this method for code which will be called each time a
 * new packet is received from the driver station and the robot is in disabled
 * mode.
 */
void IterativeRobotBase::DisabledPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

/**
 * Periodic code for autonomous mode should go here.
 *
 * Users should override this method for code which will be called each time a
 * new packet is received from the driver station and the robot is in autonomous
 * mode.
 */
void IterativeRobotBase::AutonomousPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

/**
 * Periodic code for teleop mode should go here.
 *
 * Users should override this method for code which will be called each time a
 * new packet is received from the driver station and the robot is in teleop
 * mode.
 */
void IterativeRobotBase::TeleopPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

/**
 * Periodic code for test mode should go here.
 *
 * Users should override this method for code which will be called each time a
 * new packet is received from the driver station and the robot is in test mode.
 */
void IterativeRobotBase::TestPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

void IterativeRobotBase::LoopFunc() {
  // Call the appropriate function depending upon the current robot mode
  if (IsDisabled()) {
    // Call DisabledInit() if we are now just entering disabled mode from
    // either a different mode or from power-on.
    if (m_lastMode != Mode::kDisabled) {
      LiveWindow::GetInstance()->SetEnabled(false);
      DisabledInit();
      m_lastMode = Mode::kDisabled;
    }
    HAL_ObserveUserProgramDisabled();
    DisabledPeriodic();
  } else if (IsAutonomous()) {
    // Call AutonomousInit() if we are now just entering autonomous mode from
    // either a different mode or from power-on.
    if (m_lastMode != Mode::kAutonomous) {
      LiveWindow::GetInstance()->SetEnabled(false);
      AutonomousInit();
      m_lastMode = Mode::kAutonomous;
    }
    HAL_ObserveUserProgramAutonomous();
    AutonomousPeriodic();
  } else if (IsOperatorControl()) {
    // Call TeleopInit() if we are now just entering teleop mode from
    // either a different mode or from power-on.
    if (m_lastMode != Mode::kTeleop) {
      LiveWindow::GetInstance()->SetEnabled(false);
      TeleopInit();
      m_lastMode = Mode::kTeleop;
      Scheduler::GetInstance()->SetEnabled(true);
    }
    HAL_ObserveUserProgramTeleop();
    TeleopPeriodic();
  } else {
    // Call TestInit() if we are now just entering test mode from
    // either a different mode or from power-on.
    if (m_lastMode != Mode::kTest) {
      LiveWindow::GetInstance()->SetEnabled(true);
      TestInit();
      m_lastMode = Mode::kTest;
    }
    HAL_ObserveUserProgramTest();
    TestPeriodic();
  }
  RobotPeriodic();
  SmartDashboard::UpdateValues();
  LiveWindow::GetInstance()->UpdateValues();
}
