// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/IterativeRobotBase.h"

#include <frc/DriverStation.h>

#include <hal/DriverStation.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/print.h>

#include "frc/Errors.h"
#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;

IterativeRobotBase::IterativeRobotBase(units::second_t period)
    : m_period(period),
      m_watchdog(period, [this] { PrintLoopOverrunMessage(); }) {}

void IterativeRobotBase::DriverStationConnected() {}

void IterativeRobotBase::SimulationInit() {}

void IterativeRobotBase::DisabledInit() {}

void IterativeRobotBase::AutonomousInit() {}

void IterativeRobotBase::TeleopInit() {}

void IterativeRobotBase::TestInit() {}

void IterativeRobotBase::RobotPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::print("Default {}() method... Override me!\n", __FUNCTION__);
    firstRun = false;
  }
}

void IterativeRobotBase::SimulationPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::print("Default {}() method... Override me!\n", __FUNCTION__);
    firstRun = false;
  }
}

void IterativeRobotBase::DisabledPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::print("Default {}() method... Override me!\n", __FUNCTION__);
    firstRun = false;
  }
}

void IterativeRobotBase::AutonomousPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::print("Default {}() method... Override me!\n", __FUNCTION__);
    firstRun = false;
  }
}

void IterativeRobotBase::TeleopPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::print("Default {}() method... Override me!\n", __FUNCTION__);
    firstRun = false;
  }
}

void IterativeRobotBase::TestPeriodic() {
  static bool firstRun = true;
  if (firstRun) {
    wpi::print("Default {}() method... Override me!\n", __FUNCTION__);
    firstRun = false;
  }
}

void IterativeRobotBase::DisabledExit() {}

void IterativeRobotBase::AutonomousExit() {}

void IterativeRobotBase::TeleopExit() {}

void IterativeRobotBase::TestExit() {}

void IterativeRobotBase::SetNetworkTablesFlushEnabled(bool enabled) {
  m_ntFlushEnabled = enabled;
}

units::second_t IterativeRobotBase::GetPeriod() const {
  return m_period;
}

void IterativeRobotBase::LoopFunc() {
  DriverStation::RefreshData();
  m_watchdog.Reset();

  // Get current mode; treat disabled as unknown
  hal::ControlWord word = hal::GetControlWord();
  bool enabled = word.IsEnabled();
  RobotMode mode = enabled ? word.GetRobotMode() : RobotMode::UNKNOWN;

  if (!m_calledDsConnected && word.IsDSAttached()) {
    m_calledDsConnected = true;
    DriverStationConnected();
  }

  // If mode changed, call mode exit and entry functions
  if (m_lastMode != static_cast<int>(mode)) {
    // Call last mode's exit function
    if (m_lastMode == static_cast<int>(RobotMode::UNKNOWN)) {
      DisabledExit();
    } else if (m_lastMode == static_cast<int>(RobotMode::AUTONOMOUS)) {
      AutonomousExit();
    } else if (m_lastMode == static_cast<int>(RobotMode::TELEOPERATED)) {
      TeleopExit();
    } else if (m_lastMode == static_cast<int>(RobotMode::TEST)) {
      TestExit();
    }

    // Call current mode's entry function
    if (mode == RobotMode::UNKNOWN) {
      DisabledInit();
      m_watchdog.AddEpoch("DisabledInit()");
    } else if (mode == RobotMode::AUTONOMOUS) {
      AutonomousInit();
      m_watchdog.AddEpoch("AutonomousInit()");
    } else if (mode == RobotMode::TELEOPERATED) {
      TeleopInit();
      m_watchdog.AddEpoch("TeleopInit()");
    } else if (mode == RobotMode::TEST) {
      TestInit();
      m_watchdog.AddEpoch("TestInit()");
    }

    m_lastMode = static_cast<int>(mode);
  }

  // Call the appropriate function depending upon the current robot mode
  HAL_ObserveUserProgram(word.GetValue());
  if (mode == RobotMode::UNKNOWN) {
    DisabledPeriodic();
    m_watchdog.AddEpoch("DisabledPeriodic()");
  } else if (mode == RobotMode::AUTONOMOUS) {
    AutonomousPeriodic();
    m_watchdog.AddEpoch("AutonomousPeriodic()");
  } else if (mode == RobotMode::TELEOPERATED) {
    TeleopPeriodic();
    m_watchdog.AddEpoch("TeleopPeriodic()");
  } else if (mode == RobotMode::TEST) {
    TestPeriodic();
    m_watchdog.AddEpoch("TestPeriodic()");
  }

  RobotPeriodic();
  m_watchdog.AddEpoch("RobotPeriodic()");

  SmartDashboard::UpdateValues();
  m_watchdog.AddEpoch("SmartDashboard::UpdateValues()");

  if constexpr (IsSimulation()) {
    HAL_SimPeriodicBefore();
    SimulationPeriodic();
    HAL_SimPeriodicAfter();
    m_watchdog.AddEpoch("SimulationPeriodic()");
  }

  m_watchdog.Disable();

  // Flush NetworkTables
  if (m_ntFlushEnabled) {
    nt::NetworkTableInstance::GetDefault().FlushLocal();
  }

  // Warn on loop time overruns
  if (m_watchdog.IsExpired()) {
    m_watchdog.PrintEpochs();
  }
}

void IterativeRobotBase::PrintLoopOverrunMessage() {
  FRC_ReportError(err::Error, "Loop time of {:.6f}s overrun", m_period.value());
}

void IterativeRobotBase::PrintWatchdogEpochs() {
  m_watchdog.PrintEpochs();
}
