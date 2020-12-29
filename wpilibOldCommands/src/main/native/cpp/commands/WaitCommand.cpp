// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/WaitCommand.h"

using namespace frc;

WaitCommand::WaitCommand(double timeout)
    : TimedCommand("Wait(" + std::to_string(timeout) + ")", timeout) {}

WaitCommand::WaitCommand(const wpi::Twine& name, double timeout)
    : TimedCommand(name, timeout) {}
