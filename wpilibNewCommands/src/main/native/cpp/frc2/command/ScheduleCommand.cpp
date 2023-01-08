// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/ScheduleCommand.h"

using namespace frc2;

ScheduleCommand::ScheduleCommand(std::span<Command* const> toSchedule) {
  SetInsert(m_toSchedule, toSchedule);
}

ScheduleCommand::ScheduleCommand(Command* toSchedule) {
  SetInsert(m_toSchedule, {&toSchedule, 1});
}

void ScheduleCommand::Initialize() {
  for (auto command : m_toSchedule) {
    command->Schedule();
  }
}

bool ScheduleCommand::IsFinished() {
  return true;
}

bool ScheduleCommand::RunsWhenDisabled() const {
  return true;
}
