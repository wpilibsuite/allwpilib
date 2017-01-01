/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/WaitCommand.h"

using namespace frc;

WaitCommand::WaitCommand(double timeout)
    : TimedCommand("Wait(" + std::to_string(timeout) + ")", timeout) {}

WaitCommand::WaitCommand(const std::string& name, double timeout)
    : TimedCommand(name, timeout) {}
