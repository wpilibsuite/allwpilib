/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/InstantCommand.h"

/**
 * Creates a new {@link InstantCommand} with the given name.
 * @param name the name for this command
 */
frc::InstantCommand::InstantCommand(const std::string& name) : Command(name) {}

bool frc::InstantCommand::IsFinished() { return true; }
