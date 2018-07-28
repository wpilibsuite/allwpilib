/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/InstantCommand.h"

using namespace frc;

InstantCommand::InstantCommand(const wpi::Twine& name) : Command(name) {}

bool InstantCommand::IsFinished() { return true; }
