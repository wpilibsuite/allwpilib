// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/shuffleboard/Shuffleboard.h>
#include <frc2/command/button/JoystickButton.h>

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  m_chooser.SetDefaultOption("Simple Auto", &m_simpleAuto);
  m_chooser.AddOption("Complex Auto", &m_complexAuto);

  // Put the chooser on the dashboard
  frc::Shuffleboard::GetTab("Autonomous").Add(m_chooser);

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(frc2::RunCommand(
      [this] {
        m_drive.ArcadeDrive(m_driverController.GetLeftY(),
                            m_driverController.GetRightX());
      },
      {&m_drive}));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // Grab the hatch when the 'Circle' button is pressed.
  frc2::JoystickButton(&m_driverController, frc::PS4Controller::Button::kCircle)
      .WhenPressed(&m_grabHatch);
  // Release the hatch when the 'Square' button is pressed.
  frc2::JoystickButton(&m_driverController, frc::PS4Controller::Button::kSquare)
      .WhenPressed(&m_releaseHatch);
  // While holding R1, drive at half speed
  frc2::JoystickButton(&m_driverController, frc::PS4Controller::Button::kR1)
      .WhenPressed(&m_driveHalfSpeed)
      .WhenReleased(&m_driveFullSpeed);
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return m_chooser.GetSelected();
}
