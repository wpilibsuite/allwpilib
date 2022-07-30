// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/command/PIDCommand.h>
#include <frc/command/ParallelRaceGroup.h>
#include <frc/command/RunCommand.h>
#include <frc/command/button/JoystickButton.h>
#include <frc/shuffleboard/Shuffleboard.h>

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(frc::RunCommand(
      [this] {
        m_drive.ArcadeDrive(-m_driverController.GetLeftY(),
                            m_driverController.GetRightX());
      },
      {&m_drive}));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // Assorted commands to be bound to buttons

  // Stabilize robot to drive straight with gyro when L1 is held
  frc::JoystickButton(&m_driverController, frc::PS4Controller::Button::kL1)
      .WhenHeld(frc::PIDCommand{
          frc::PIDController{dc::kStabilizationP, dc::kStabilizationI,
                             dc::kStabilizationD},
          // Close the loop on the turn rate
          [this] { return m_drive.GetTurnRate(); },
          // Setpoint is 0
          0,
          // Pipe the output to the turning controls
          [this](double output) {
            m_drive.ArcadeDrive(m_driverController.GetLeftY(), output);
          },
          // Require the robot drive
          {&m_drive}});

  // Turn to 90 degrees when the 'Cross' button is pressed
  frc::JoystickButton(&m_driverController, frc::PS4Controller::Button::kCross)
      .WhenPressed(TurnToAngle{90_deg, &m_drive}.WithTimeout(5_s));

  // Turn to -90 degrees with a profile when the 'Square' button is pressed,
  // with a 5 second timeout
  frc::JoystickButton(&m_driverController, frc::PS4Controller::Button::kSquare)
      .WhenPressed(TurnToAngle{90_deg, &m_drive}.WithTimeout(5_s));

  // While holding R1, drive at half speed
  frc::JoystickButton(&m_driverController, frc::PS4Controller::Button::kR1)
      .WhenPressed(frc::InstantCommand{[this] { m_drive.SetMaxOutput(0.5); }})
      .WhenReleased(frc::InstantCommand{[this] { m_drive.SetMaxOutput(1); }});
}

frc::Command* RobotContainer::GetAutonomousCommand() {
  // no auto
  return nullptr;
}
