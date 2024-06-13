// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc2/command/Commands.h>

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(frc2::cmd::Run(
      [this] {
        m_drive.ArcadeDrive(-m_driverController.GetLeftY(),
                            -m_driverController.GetRightX());
      },
      {&m_drive}));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // While holding the shoulder button, drive at half speed
  m_driverController.RightBumper()
      .OnTrue(m_driveHalfSpeed.get())
      .OnFalse(m_driveFullSpeed.get());

  // Drive forward by 3 meters when the 'A' button is pressed, with a timeout of
  // 10 seconds
  m_driverController.A().OnTrue(
      m_drive.ProfiledDriveDistance(3_m).WithTimeout(10_s));

  // Do the same thing as above when the 'B' button is pressed, but without
  // resetting the encoders
  m_driverController.B().OnTrue(
      m_drive.DynamicProfiledDriveDistance(3_m).WithTimeout(10_s));
}

frc2::CommandPtr RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return frc2::cmd::None();
}
