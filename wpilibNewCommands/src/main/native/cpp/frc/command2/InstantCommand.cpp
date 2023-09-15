// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command2/InstantCommand.h"

using namespace frc;

InstantCommand::InstantCommand(std::function<void()> toRun,
                               Requirements requirements)
    : CommandHelper(
          std::move(toRun), [] {}, [](bool interrupted) {}, [] { return true; },
          requirements) {}

InstantCommand::InstantCommand() : InstantCommand([] {}) {}
