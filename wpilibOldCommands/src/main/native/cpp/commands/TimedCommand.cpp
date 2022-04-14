// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/TimedCommand.h"

using namespace frc;

TimedCommand::TimedCommand(std::string_view name, units::second_t timeout)
    : Command(name, timeout) {}

TimedCommand::TimedCommand(units::second_t timeout) : Command(timeout) {}

TimedCommand::TimedCommand(std::string_view name, units::second_t timeout,
                           Subsystem& subsystem)
    : Command(name, timeout, subsystem) {}

TimedCommand::TimedCommand(units::second_t timeout, Subsystem& subsystem)
    : Command(timeout, subsystem) {}

bool TimedCommand::IsFinished() {
  return IsTimedOut();
}
