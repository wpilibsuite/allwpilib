// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "commands/TeleopArcadeDrive.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

RobotContainer::RobotContainer() {
  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Also set default commands here
  drive.SetDefaultCommand(TeleopArcadeDrive(
      &drive, [this] { return -controller.GetRawAxis(1); },
      [this] { return -controller.GetRawAxis(2); }));

  // Example of how to use the onboard IO
  onboardButtonA.OnTrue(wpi::cmd::Print("Button A Pressed"))
      .OnFalse(wpi::cmd::Print("Button A Released"));

  // Setup SmartDashboard options.
  chooser.SetDefaultOption("Auto Routine Distance", &autoDistance);
  chooser.AddOption("Auto Routine Time", &autoTime);
  wpi::SmartDashboard::PutData("Auto Selector", &chooser);
}

wpi::cmd::Command* RobotContainer::GetAutonomousCommand() {
  return chooser.GetSelected();
}
