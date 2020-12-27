// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc2/command/PrintCommand.h>
#include <frc2/command/button/Button.h>

#include "Constants.h"
#include "commands\TeleopArcadeDrive.h"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Also set default commands here
  m_drive.SetDefaultCommand(TeleopArcadeDrive(
      &m_drive, [this] { return m_controller.GetRawAxis(1); },
      [this] { return m_controller.GetRawAxis(2); }));

  // Example of how to use the onboard IO
  frc2::Button onboardButtonA{[this] { return m_onboardIO.GetButtonAPressed(); }};
  onboardButtonA
      .WhenPressed(PrintCommand("Button A Pressed"))
      .WhenReleased(PrintCommand("Button A Released"));
}

frc2::Command* RobotContainer::GetAutonomousCommand() { return nullptr; }
