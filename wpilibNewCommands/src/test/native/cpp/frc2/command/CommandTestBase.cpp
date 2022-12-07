// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"

using namespace frc2;

CommandTestBase::CommandTestBase() {
  auto& scheduler = CommandScheduler::GetInstance();
  scheduler.CancelAll();
  scheduler.Enable();
  scheduler.GetActiveButtonLoop()->Clear();
}

CommandScheduler CommandTestBase::GetScheduler() {
  return CommandScheduler();
}

void CommandTestBase::SetUp() {
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();
}

void CommandTestBase::TearDown() {
  CommandScheduler::GetInstance().GetActiveButtonLoop()->Clear();
}

void CommandTestBase::SetDSEnabled(bool enabled) {
  frc::sim::DriverStationSim::SetEnabled(enabled);
  frc::sim::DriverStationSim::NotifyNewData();
}
