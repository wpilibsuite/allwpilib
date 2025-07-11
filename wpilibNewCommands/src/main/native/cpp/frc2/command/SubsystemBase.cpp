// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/SubsystemBase.h"

#include <string>

#include <wpi/telemetry/TelemetryTable.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandScheduler.h"

using namespace frc2;

SubsystemBase::SubsystemBase() {
  CommandScheduler::GetInstance().RegisterSubsystem({this});
}

SubsystemBase::SubsystemBase(std::string_view name) : m_name{name} {
  CommandScheduler::GetInstance().RegisterSubsystem({this});
}

void SubsystemBase::UpdateTelemetry(wpi::TelemetryTable& table) const {
  auto defaultCommand = GetDefaultCommand();
  table.Log(".hasDefault", defaultCommand != nullptr);
  table.Log(".default", defaultCommand ? defaultCommand->GetName() : "none");
  auto currentCommand = GetCurrentCommand();
  table.Log(".hasCommand", currentCommand != nullptr);
  table.Log(".command", currentCommand ? currentCommand->GetName() : "none");
}

std::string_view SubsystemBase::GetTelemetryType() const {
  return "Subsystem";
}

std::string SubsystemBase::GetName() const {
  return m_name;
}

void SubsystemBase::SetName(std::string_view name) {
  m_name = name;
}
