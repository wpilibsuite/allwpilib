/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "IterativeRobotBase.h"

#include <cstdio>

#include <HAL/HAL.h>
#include <wpi/raw_ostream.h>

#include "Commands/Scheduler.h"
#include "LiveWindow/LiveWindow.h"
#include "SmartDashboard/SmartDashboard.h"

using namespace frc;

void IterativeRobotBase::RobotInit() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void IterativeRobotBase::DisabledInit() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void IterativeRobotBase::AutonomousInit() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void IterativeRobotBase::TeleopInit() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void IterativeRobotBase::TestInit() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void IterativeRobotBase::RobotPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

void IterativeRobotBase::DisabledPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

void IterativeRobotBase::AutonomousPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

void IterativeRobotBase::TeleopPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
    firstRun = false;
  }
}

void IterativeRobotBase::TestPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
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
