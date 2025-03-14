// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/IterativeRobotBase.h"

#include <frc/DriverStation.h>

#include <hal/DriverStation.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/print.h>

#include "frc/DSControlWord.h"
#include "frc/Errors.h"
#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;

IterativeRobotBase::IterativeRobotBase(units::second_t period)
    : m_autonomousMode(DriverStation::AddOpModeOption("auto", "", "", 0)),
      m_teleoperatedMode(DriverStation::AddOpModeOption("teleop", "", "", 0)),
      m_period(period),
      m_watchdog(period, [this] { PrintLoopOverrunMessage(); }) {}

void IterativeRobotBase::RobotInit() {}

void IterativeRobotBase::DriverStationConnected() {}

void IterativeRobotBase::SimulationInit() {}

void IterativeRobotBase::DisabledInit() {}

void IterativeRobotBase::AutonomousInit() {}

void IterativeRobotBase::TeleopInit() {}

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

void IterativeRobotBase::DisabledExit() {}

void IterativeRobotBase::AutonomousExit() {}

void IterativeRobotBase::TeleopExit() {}

bool IterativeRobotBase::IsAutonomous() const {
  return DriverStation::GetOpModeId() == m_autonomousMode;
}

bool IterativeRobotBase::IsTeleoperated() const {
  return DriverStation::GetOpModeId() == m_teleoperatedMode;
}

void IterativeRobotBase::SetNetworkTablesFlushEnabled(bool enabled) {
  m_ntFlushEnabled = enabled;
}

units::second_t IterativeRobotBase::GetPeriod() const {
  return m_period;
}

void IterativeRobotBase::LoopFunc() {
  DriverStation::RefreshData();
  m_watchdog.Reset();

  // Get current mode
  DSControlWord word;
  int mode = DriverStation::GetOpModeId();

  if (!m_calledDsConnected && word.IsDSAttached()) {
    m_calledDsConnected = true;
    DriverStationConnected();
  }

  // If mode changed, call mode exit and entry functions
  if (m_lastMode != mode) {
    // Call last mode's exit function
    if (m_lastMode == 0) {
      DisabledExit();
    } else if (m_lastMode == m_autonomousMode) {
      AutonomousExit();
    } else if (m_lastMode == m_teleoperatedMode) {
      TeleopExit();
    }

    // Call current mode's entry function
    if (mode == 0) {
      DisabledInit();
      m_watchdog.AddEpoch("DisabledInit()");
    } else if (mode == m_autonomousMode) {
      AutonomousInit();
      m_watchdog.AddEpoch("AutonomousInit()");
    } else if (mode == m_teleoperatedMode) {
      TeleopInit();
      m_watchdog.AddEpoch("TeleopInit()");
    }

    m_lastMode = mode;
  }

  // Call the appropriate function depending upon the current robot mode
  HAL_ObserveUserProgramOpMode(mode);
  if (mode == 0) {
    DisabledPeriodic();
    m_watchdog.AddEpoch("DisabledPeriodic()");
  } else if (mode == m_autonomousMode) {
    AutonomousPeriodic();
    m_watchdog.AddEpoch("AutonomousPeriodic()");
  } else if (mode == m_teleoperatedMode) {
    TeleopPeriodic();
    m_watchdog.AddEpoch("TeleopPeriodic()");
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
