// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/TimedCommand.h"

using namespace frc;

TimedCommand::TimedCommand(const wpi::Twine& name, double timeout)
    : Command(name, timeout) {}

TimedCommand::TimedCommand(double timeout) : Command(timeout) {}

TimedCommand::TimedCommand(const wpi::Twine& name, double timeout,
                           Subsystem& subsystem)
    : Command(name, timeout, subsystem) {}

TimedCommand::TimedCommand(double timeout, Subsystem& subsystem)
    : Command(timeout, subsystem) {}

bool TimedCommand::IsFinished() { return IsTimedOut(); }
