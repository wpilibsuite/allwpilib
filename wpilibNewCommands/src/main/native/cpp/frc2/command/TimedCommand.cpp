// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/TimedCommand.h"

using namespace frc2;

TimedCommand::TimedCommand(std::function<void(units::second_t)> toRun,
                           std::initializer_list<Subsystem*> requirements)
    : m_output(toRun) {
  this->AddRequirements(requirements);
}

TimedCommand::TimedCommand(std::function<void(units::second_t)> toRun,
                           std::span<Subsystem* const> requirements)
    : m_output(toRun) {
  this->AddRequirements(requirements);
}
