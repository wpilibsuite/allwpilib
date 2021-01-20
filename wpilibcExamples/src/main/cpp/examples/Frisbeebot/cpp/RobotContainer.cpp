// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/shuffleboard/Shuffleboard.h>
#include <frc2/command/button/JoystickButton.h>

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(frc2::RunCommand(
      [this] {
        m_drive.ArcadeDrive(
            -m_driverController.GetY(frc::GenericHID::kLeftHand),
            m_driverController.GetX(frc::GenericHID::kRightHand));
      },
      {&m_drive}));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // Spin up the shooter when the 'A' button is pressed
  frc2::JoystickButton(&m_driverController, 1).WhenPressed(&m_spinUpShooter);

  // Turn off the shooter when the 'B' button is pressed
  frc2::JoystickButton(&m_driverController, 2).WhenPressed(&m_stopShooter);

  // Shoot when the 'X' button is held
  frc2::JoystickButton(&m_driverController, 3)
      .WhenPressed(&m_shoot)
      .WhenReleased(&m_stopFeeder);

  // While holding the shoulder button, drive at half speed
  frc2::JoystickButton(&m_driverController, 6)
      .WhenPressed(&m_driveHalfSpeed)
      .WhenReleased(&m_driveFullSpeed);
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return &m_autonomousCommand;
}
