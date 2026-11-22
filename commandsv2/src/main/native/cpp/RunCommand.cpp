// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/RunCommand.hpp"

#include <utility>

using namespace wpi::cmd;

RunCommand::RunCommand(std::function<void()> toRun, Requirements requirements)
    : CommandHelper([] {}, std::move(toRun), [](bool interrupted) {},
                    [] { return false; }, requirements) {}
