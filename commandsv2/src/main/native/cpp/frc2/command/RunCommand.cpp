// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/RunCommand.h"

#include <utility>

using namespace frc2;

RunCommand::RunCommand(std::function<void()> toRun, Requirements requirements)
    : CommandHelper([] {}, std::move(toRun), [](bool interrupted) {},
                    [] { return false; }, requirements) {}
