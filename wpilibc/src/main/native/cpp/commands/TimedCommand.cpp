/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
