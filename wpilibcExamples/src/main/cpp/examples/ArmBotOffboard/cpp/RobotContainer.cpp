// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <units/angle.h>

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(m_drive.ArcadeDriveCommand(
      [this] { return -m_driverController.GetLeftY(); },
      [this] { return -m_driverController.GetRightX(); }));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // Move the arm to 2 radians above horizontal when the 'A' button is pressed.
  m_driverController.A().OnTrue(m_arm.SetArmGoalCommand(2_rad));

  // Move the arm to neutral position when the 'B' button is pressed.
  m_driverController.B().OnTrue(
      m_arm.SetArmGoalCommand(ArmConstants::kArmOffset));

  // While holding the shoulder button, drive at half speed
  m_driverController.RightBumper()
      .OnTrue(m_drive.SetMaxOutputCommand(0.5))
      .OnFalse(m_drive.SetMaxOutputCommand(1.0));
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  return nullptr;
}
