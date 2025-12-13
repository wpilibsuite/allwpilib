// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "wpi/commands2/Commands.hpp"

RobotContainer::RobotContainer() {
  ConfigureBindings();
}

void RobotContainer::ConfigureBindings() {}

wpi::cmd::CommandPtr RobotContainer::GetAutonomousCommand() {
  return wpi::cmd::cmd::Print("No autonomous command configured");
}
