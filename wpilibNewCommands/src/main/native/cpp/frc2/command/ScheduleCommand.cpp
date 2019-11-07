/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/ScheduleCommand.h"

using namespace frc2;

ScheduleCommand::ScheduleCommand(wpi::ArrayRef<Command*> toSchedule) {
  SetInsert(m_toSchedule, toSchedule);
}

void ScheduleCommand::Initialize() {
  for (auto command : m_toSchedule) {
    command->Schedule();
  }
}

bool ScheduleCommand::IsFinished() { return true; }

bool ScheduleCommand::RunsWhenDisabled() const { return true; }
