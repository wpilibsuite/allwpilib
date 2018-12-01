/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/IterativeRobotBase.h"

#include <cstdio>

#include <hal/HAL.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"
#include "frc/Timer.h"
#include "frc/commands/Scheduler.h"
#include "frc/livewindow/LiveWindow.h"
#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;

IterativeRobotBase::IterativeRobotBase(double period)
    : m_period(period),
      m_watchdog(period, [this] { PrintLoopOverrunMessage(); }) {}

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
  m_watchdog.Reset();

  // Call the appropriate function depending upon the current robot mode
  if (IsDisabled()) {
    // Call DisabledInit() if we are now just entering disabled mode from
    // either a different mode or from power-on.
    if (m_lastMode != Mode::kDisabled) {
      LiveWindow::GetInstance()->SetEnabled(false);
      DisabledInit();
      m_watchdog.AddEpoch("DisabledInit()");
      m_lastMode = Mode::kDisabled;
    }

    HAL_ObserveUserProgramDisabled();
    DisabledPeriodic();
    m_watchdog.AddEpoch("DisabledPeriodic()");
  } else if (IsAutonomous()) {
    // Call AutonomousInit() if we are now just entering autonomous mode from
    // either a different mode or from power-on.
    if (m_lastMode != Mode::kAutonomous) {
      LiveWindow::GetInstance()->SetEnabled(false);
      AutonomousInit();
      m_watchdog.AddEpoch("AutonomousInit()");
      m_lastMode = Mode::kAutonomous;
    }

    HAL_ObserveUserProgramAutonomous();
    AutonomousPeriodic();
    m_watchdog.AddEpoch("AutonomousPeriodic()");
  } else if (IsOperatorControl()) {
    // Call TeleopInit() if we are now just entering teleop mode from
    // either a different mode or from power-on.
    if (m_lastMode != Mode::kTeleop) {
      LiveWindow::GetInstance()->SetEnabled(false);
      TeleopInit();
      m_watchdog.AddEpoch("TeleopInit()");
      m_lastMode = Mode::kTeleop;
      Scheduler::GetInstance()->SetEnabled(true);
    }

    HAL_ObserveUserProgramTeleop();
    TeleopPeriodic();
    m_watchdog.AddEpoch("TeleopPeriodic()");
  } else {
    // Call TestInit() if we are now just entering test mode from
    // either a different mode or from power-on.
    if (m_lastMode != Mode::kTest) {
      LiveWindow::GetInstance()->SetEnabled(true);
      TestInit();
      m_watchdog.AddEpoch("TestInit()");
      m_lastMode = Mode::kTest;
    }

    HAL_ObserveUserProgramTest();
    TestPeriodic();
    m_watchdog.AddEpoch("TestPeriodic()");
  }

  RobotPeriodic();
  m_watchdog.AddEpoch("RobotPeriodic()");
  m_watchdog.Disable();
  SmartDashboard::UpdateValues();

  LiveWindow::GetInstance()->UpdateValues();

  // Warn on loop time overruns
  if (m_watchdog.IsExpired()) {
    m_watchdog.PrintEpochs();
  }
}

void IterativeRobotBase::PrintLoopOverrunMessage() {
  wpi::SmallString<128> str;
  wpi::raw_svector_ostream buf(str);

  buf << "Loop time of " << m_period << "s overrun\n";

  DriverStation::ReportWarning(str);
}
