// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/WaitCommand.hpp"

#include <format>

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi::cmd;

WaitCommand::WaitCommand(wpi::units::second_t duration) : m_duration{duration} {
  SetName(std::format("{}: {}", GetName(), duration));
}

void WaitCommand::Initialize() {
  m_timer.Restart();
}

void WaitCommand::End(bool interrupted) {
  m_timer.Stop();
}

bool WaitCommand::IsFinished() {
  return m_timer.HasElapsed(m_duration);
}

bool WaitCommand::RunsWhenDisabled() const {
  return true;
}

void WaitCommand::LogTo(wpi::TelemetryTable& table) const {
  Command::LogTo(table);
  table.Log("duration", m_duration);
}
