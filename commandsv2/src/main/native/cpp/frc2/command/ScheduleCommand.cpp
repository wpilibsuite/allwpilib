// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/ScheduleCommand.hpp"

using namespace frc2;

ScheduleCommand::ScheduleCommand(std::span<Command* const> toSchedule) {
  for (auto cmd : toSchedule) {
    m_toSchedule.insert(cmd);
  }
}

ScheduleCommand::ScheduleCommand(Command* toSchedule) {
  m_toSchedule.insert(toSchedule);
}

void ScheduleCommand::Initialize() {
  for (auto command : m_toSchedule) {
    frc2::CommandScheduler::GetInstance().Schedule(command);
  }
}

bool ScheduleCommand::IsFinished() {
  return true;
}

bool ScheduleCommand::RunsWhenDisabled() const {
  return true;
}
