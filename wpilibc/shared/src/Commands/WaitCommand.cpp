/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/WaitCommand.h"

using namespace frc;

/**
 * Creates a new WaitCommand with the given name and timeout.
 *
 * @param name    the name of the command
 * @param timeout the time (in seconds) before this command "times out"
 */
WaitCommand::WaitCommand(double timeout)
    : TimedCommand("Wait(" + std::to_string(timeout) + ")", timeout) {}

/**
 * Creates a new WaitCommand with the given timeout.
 *
 * @param timeout the time (in seconds) before this command "times out"
 */
WaitCommand::WaitCommand(const std::string& name, double timeout)
    : TimedCommand(name, timeout) {}
