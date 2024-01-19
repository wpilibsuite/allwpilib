// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/WaitCommand.h"

#include <fmt/format.h>
#include <wpi/sendable/SendableBuilder.h>

using namespace frc2;

WaitCommand::WaitCommand(units::second_t duration)
    : WaitCommand{[duration] { return duration; }} {
  SetName(fmt::format("{}: {}", GetName(), duration));
}

WaitCommand::WaitCommand(std::function<units::second_t()> durationSupplier)
    : m_durationSupplier{durationSupplier} {}

void WaitCommand::Initialize() {
  m_duration = m_durationSupplier();
  m_timer.Restart();
}

void WaitCommand::End(bool interrupted) {
  m_timer.Stop();
  m_duration = 0_s;
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
