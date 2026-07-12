// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "commands/TeleopArcadeDrive.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/button/JoystickButton.hpp"
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
  userButton.OnTrue(wpi::cmd::Print("USER Button Pressed"))
      .OnFalse(wpi::cmd::Print("USER Button Released"));

  wpi::cmd::JoystickButton(&controller, 1)
      .OnTrue(wpi::cmd::RunOnce([this] { arm.SetAngle(45_deg); }, {}))
      .OnFalse(wpi::cmd::RunOnce([this] { arm.SetAngle(0_deg); }, {}));

  wpi::cmd::JoystickButton(&controller, 2)
      .OnTrue(wpi::cmd::RunOnce([this] { arm.SetAngle(90_deg); }, {}))
      .OnFalse(wpi::cmd::RunOnce([this] { arm.SetAngle(0_deg); }, {}));

  // Setup SmartDashboard options.
  chooser.SetDefaultOption("Auto Routine Distance", &autoDistance);
  chooser.AddOption("Auto Routine Time", &autoTime);
  wpi::SmartDashboard::PutData("Auto Selector", &chooser);
}

wpi::cmd::Command* RobotContainer::GetAutonomousCommand() {
  return chooser.GetSelected();
}
