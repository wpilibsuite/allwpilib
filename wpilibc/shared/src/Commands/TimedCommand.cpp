/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/TimedCommand.h"

/**
 * Creates a new TimedCommand with the given name and timeout.
 *
 * @param name    the name of the command
 * @param timeout the time (in seconds) before this command "times out"
 */
frc::TimedCommand::TimedCommand(const std::string& name, double timeout)
    : Command(name, timeout) {}

/**
 * Creates a new WaitCommand with the given timeout.
 *
 * @param timeout the time (in seconds) before this command "times out"
 */
frc::TimedCommand::TimedCommand(double timeout) : Command(timeout) {}

/**
 * Ends command when timed out.
 */
bool frc::TimedCommand::IsFinished() { return IsTimedOut(); }
