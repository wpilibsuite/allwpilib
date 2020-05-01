/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RobotContainer.h"

#include <frc/shuffleboard/Shuffleboard.h>
#include <frc2/command/PIDCommand.h>
#include <frc2/command/ParallelRaceGroup.h>
#include <frc2/command/RunCommand.h>
#include <frc2/command/button/JoystickButton.h>

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(frc2::RunCommand(
      [this] {
        m_drive.ArcadeDrive(
            m_driverController.GetY(frc::GenericHID::kLeftHand),
            m_driverController.GetX(frc::GenericHID::kRightHand));
      },
      {&m_drive}));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // Assorted commands to be bound to buttons

  // Stabilize robot to drive straight with gyro when left bumper is held
  frc2::JoystickButton(&m_driverController, 5)
      .WhenHeld(frc2::PIDCommand{
          frc2::PIDController{dc::kStabilizationP, dc::kStabilizationI,
                              dc::kStabilizationD},
          // Close the loop on the turn rate
          [this] { return m_drive.GetTurnRate(); },
          // Setpoint is 0
          0,
          // Pipe the output to the turning controls
          [this](double output) {
            m_drive.ArcadeDrive(m_driverController.GetY(
                                    frc::GenericHID::JoystickHand::kLeftHand),
                                output);
          },
          // Require the robot drive
          {&m_drive}});

  // Turn to 90 degrees when the 'X' button is pressed
  frc2::JoystickButton(&m_driverController, 3)
      .WhenPressed(TurnToAngle{90_deg, &m_drive}.WithTimeout(5_s));

  // Turn to -90 degrees with a profile when the 'A' button is pressed, with a 5
  // second timeout
  frc2::JoystickButton(&m_driverController, 1)
      .WhenPressed(TurnToAngle{90_deg, &m_drive}.WithTimeout(5_s));

  // While holding the shoulder button, drive at half speed
  frc2::JoystickButton(&m_driverController, 6)
      .WhenPressed(frc2::InstantCommand{[this] { m_drive.SetMaxOutput(0.5); }})
      .WhenReleased(frc2::InstantCommand{[this] { m_drive.SetMaxOutput(1); }});
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // no auto
  return nullptr;
}
