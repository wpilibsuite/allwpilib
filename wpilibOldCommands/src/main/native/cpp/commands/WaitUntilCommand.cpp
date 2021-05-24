// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/WaitUntilCommand.h"

#include "frc/Timer.h"

using namespace frc;

WaitUntilCommand::WaitUntilCommand(double time)
    : Command("WaitUntilCommand", time) {
  m_time = time;
}

WaitUntilCommand::WaitUntilCommand(std::string_view name, double time)
    : Command(name, time) {
  m_time = time;
}

bool WaitUntilCommand::IsFinished() {
  return Timer::GetMatchTime() >= m_time;
}
