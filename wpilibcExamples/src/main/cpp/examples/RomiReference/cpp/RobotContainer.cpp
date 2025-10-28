// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include <wpi/commands2/Commands.hpp>
#include <wpi/smartdashboard/SmartDashboard.hpp>

#include "commands/TeleopArcadeDrive.hpp"

RobotContainer::RobotContainer() {
  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Also set default commands here
  m_drive.SetDefaultCommand(TeleopArcadeDrive(
      &m_drive, [this] { return -m_controller.GetRawAxis(1); },
      [this] { return -m_controller.GetRawAxis(2); }));

  // Example of how to use the onboard IO
  m_onboardButtonA.OnTrue(wpi::cmd::cmd::Print("Button A Pressed"))
      .OnFalse(wpi::cmd::cmd::Print("Button A Released"));

  // Setup SmartDashboard options.
  m_chooser.SetDefaultOption("Auto Routine Distance", &m_autoDistance);
  m_chooser.AddOption("Auto Routine Time", &m_autoTime);
  wpi::SmartDashboard::PutData("Auto Selector", &m_chooser);
}

wpi::cmd::Command* RobotContainer::GetAutonomousCommand() {
  return m_chooser.GetSelected();
}
