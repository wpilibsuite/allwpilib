// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/WaitCommand.h"

#include <fmt/format.h>
#include <frc/fmt/Units.h>

using namespace frc2;

WaitCommand::WaitCommand(units::second_t duration) : m_duration{duration} {
  SetName(fmt::format("{}: {}", GetName(), duration));
}

void WaitCommand::Initialize() {
  m_timer.Reset();
  m_timer.Start();
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
