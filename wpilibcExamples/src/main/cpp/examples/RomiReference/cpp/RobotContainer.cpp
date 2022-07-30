// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/command/PrintCommand.h>
#include <frc/command/button/Button.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include "commands/TeleopArcadeDrive.h"

RobotContainer::RobotContainer() {
  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Also set default commands here
  m_drive.SetDefaultCommand(TeleopArcadeDrive(
      &m_drive, [this] { return -m_controller.GetRawAxis(1); },
      [this] { return m_controller.GetRawAxis(2); }));

  // Example of how to use the onboard IO
  m_onboardButtonA.WhenPressed(frc::PrintCommand("Button A Pressed"))
      .WhenReleased(frc::PrintCommand("Button A Released"));

  // Setup SmartDashboard options.
  m_chooser.SetDefaultOption("Auto Routine Distance", &m_autoDistance);
  m_chooser.AddOption("Auto Routine Time", &m_autoTime);
  frc::SmartDashboard::PutData("Auto Selector", &m_chooser);
}

frc::Command* RobotContainer::GetAutonomousCommand() {
  return m_chooser.GetSelected();
}
