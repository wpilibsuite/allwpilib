/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/InstantCommand.h"

using namespace frc2;

InstantCommand::InstantCommand(std::function<void()> toRun,
                               std::initializer_list<Subsystem*> requirements)
    : m_toRun{std::move(toRun)} {
  AddRequirements(requirements);
}

InstantCommand::InstantCommand(std::function<void()> toRun,
                               wpi::ArrayRef<Subsystem*> requirements)
    : m_toRun{std::move(toRun)} {
  AddRequirements(requirements);
}

InstantCommand::InstantCommand() : m_toRun{[] {}} {}

void InstantCommand::Initialize() { m_toRun(); }

bool InstantCommand::IsFinished() { return true; }
