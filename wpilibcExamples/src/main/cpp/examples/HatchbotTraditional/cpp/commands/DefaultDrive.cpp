// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DefaultDrive.hpp"

#include <utility>

DefaultDrive::DefaultDrive(DriveSubsystem* subsystem,
                           std::function<double()> forward,
                           std::function<double()> rotation)
    : drive{subsystem},
      forward{std::move(forward)},
      rotation{std::move(rotation)} {
  AddRequirements(subsystem);
}

void DefaultDrive::Execute() {
  drive->ArcadeDrive(forward(), rotation());
}
