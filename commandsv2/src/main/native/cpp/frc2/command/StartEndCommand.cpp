// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/StartEndCommand.hpp"

#include <utility>

using namespace frc2;

StartEndCommand::StartEndCommand(std::function<void()> onInit,
                                 std::function<void()> onEnd,
                                 Requirements requirements)
    : CommandHelper(
          std::move(onInit), [] {},
          [onEnd = std::move(onEnd)](bool interrupted) { onEnd(); },
          [] { return false; }, requirements) {}
