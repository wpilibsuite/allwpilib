// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/WaitUntilCommand.hpp"

#include <utility>

#include "wpi/system/Timer.hpp"

using namespace wpi::cmd;

WaitUntilCommand::WaitUntilCommand(std::function<bool()> condition)
    : m_condition{std::move(condition)} {}

WaitUntilCommand::WaitUntilCommand(wpi::units::second_t time)
    : m_condition{[=] { return wpi::Timer::GetMatchTime() - time > 0_s; }} {}

bool WaitUntilCommand::IsFinished() {
  return m_condition();
}

bool WaitUntilCommand::RunsWhenDisabled() const {
  return true;
}
