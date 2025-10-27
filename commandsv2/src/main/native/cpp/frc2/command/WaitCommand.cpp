// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/WaitCommand.h"

#include <fmt/format.h>
#include <wpi/sendable/SendableBuilder.h>

using namespace frc2;

WaitCommand::WaitCommand(units::second_t duration) : m_duration{duration} {
  SetName(fmt::format("{}: {}", GetName(), duration));
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

void WaitCommand::InitSendable(wpi::SendableBuilder& builder) {
  Command::InitSendable(builder);
  builder.AddDoubleProperty(
      "duration", [this] { return m_duration.value(); }, nullptr);
}
