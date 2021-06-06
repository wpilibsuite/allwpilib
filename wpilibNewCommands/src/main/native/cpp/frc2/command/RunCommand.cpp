// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/RunCommand.h"

using namespace frc2;

RunCommand::RunCommand(std::function<void()> toRun,
                       std::initializer_list<Subsystem*> requirements)
    : m_toRun{std::move(toRun)} {
  AddRequirements(requirements);
}

RunCommand::RunCommand(std::function<void()> toRun,
                       wpi::span<Subsystem* const> requirements)
    : m_toRun{std::move(toRun)} {
  AddRequirements(requirements);
}

void RunCommand::Execute() {
  m_toRun();
}
