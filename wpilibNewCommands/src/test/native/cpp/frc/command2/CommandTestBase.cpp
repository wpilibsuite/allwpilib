// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"

CommandTestBase::CommandTestBase() {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  scheduler.CancelAll();
  scheduler.Enable();
  scheduler.GetActiveButtonLoop()->Clear();
  scheduler.UnregisterAllSubsystems();

  SetDSEnabled(true);
}

CommandTestBase::~CommandTestBase() {
  frc::CommandScheduler::GetInstance().GetActiveButtonLoop()->Clear();
  frc::CommandScheduler::GetInstance().UnregisterAllSubsystems();
}

frc::CommandScheduler CommandTestBase::GetScheduler() {
  return frc::CommandScheduler();
}

void CommandTestBase::SetDSEnabled(bool enabled) {
  frc::sim::DriverStationSim::SetDsAttached(true);
  frc::sim::DriverStationSim::SetEnabled(enabled);
  frc::sim::DriverStationSim::NotifyNewData();
}
