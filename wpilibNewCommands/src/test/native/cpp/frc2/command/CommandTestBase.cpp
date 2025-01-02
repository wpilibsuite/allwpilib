// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"

using namespace frc2;

CommandTestBase::CommandTestBase() {
  CommandScheduler::ResetInstance();

  SetDSEnabled(true);
}

CommandTestBase::~CommandTestBase() {
  CommandScheduler::GetInstance().GetActiveButtonLoop()->Clear();
  CommandScheduler::GetInstance().UnregisterAllSubsystems();
}

void CommandTestBase::SetDSEnabled(bool enabled) {
  frc::sim::DriverStationSim::SetDsAttached(true);
  frc::sim::DriverStationSim::SetEnabled(enabled);
  frc::sim::DriverStationSim::NotifyNewData();
}
