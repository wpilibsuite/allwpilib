// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/WaitUntilCommand.h"

#include <frc/Timer.h>

using namespace frc2;

WaitUntilCommand::WaitUntilCommand(std::function<bool()> condition)
    : m_condition{std::move(condition)} {}

WaitUntilCommand::WaitUntilCommand(units::second_t time)
    : m_condition{[=] { return frc::Timer::GetMatchTime() - time > 0_s; }} {}

bool WaitUntilCommand::IsFinished() {
  return m_condition();
}

bool WaitUntilCommand::RunsWhenDisabled() const {
  return true;
}
