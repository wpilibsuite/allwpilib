/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/RunCommand.h"

using namespace frc2;

RunCommand::RunCommand(std::function<void()> toRun,
                       std::initializer_list<Subsystem*> requirements)
    : m_toRun{std::move(toRun)} {
  AddRequirements(requirements);
}

RunCommand::RunCommand(std::function<void()> toRun,
                       wpi::ArrayRef<Subsystem*> requirements)
    : m_toRun{std::move(toRun)} {
  AddRequirements(requirements);
}

void RunCommand::Execute() { m_toRun(); }
