/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/TimedCommand.h"

using namespace frc;

TimedCommand::TimedCommand(const std::string& name, double timeout)
    : Command(name, timeout) {}

TimedCommand::TimedCommand(double timeout) : Command(timeout) {}

bool TimedCommand::IsFinished() { return IsTimedOut(); }
