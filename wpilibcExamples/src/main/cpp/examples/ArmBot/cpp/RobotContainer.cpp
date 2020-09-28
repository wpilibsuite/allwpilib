/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RobotContainer.h"

#include <frc/shuffleboard/Shuffleboard.h>
#include <frc2/command/button/JoystickButton.h>
#include <units/angle.h>

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

  // Move the arm to 2 radians above horizontal when the 'A' button is pressed.
  frc2::JoystickButton(&m_driverController, 1)
      .WhenPressed(
          [this] {
            m_arm.SetGoal(2_rad);
            m_arm.Enable();
          },
          {&m_arm});

  // Move the arm to neutral position when the 'B' button is pressed.
  frc2::JoystickButton(&m_driverController, 2)
      .WhenPressed(
          [this] {
            m_arm.SetGoal(ArmConstants::kArmOffset);
            m_arm.Enable();
          },
          {&m_arm});

  // Disable the arm controller when Y is pressed.
  frc2::JoystickButton(&m_driverController, 4)
      .WhenPressed([this] { m_arm.Disable(); }, {&m_arm});

  // While holding the shoulder button, drive at half speed
  frc2::JoystickButton(&m_driverController, 6)
      .WhenPressed([this] { m_drive.SetMaxOutput(.5); })
      .WhenReleased([this] { m_drive.SetMaxOutput(1); });
}

void RobotContainer::DisablePIDSubsystems() { m_arm.Disable(); }

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return new frc2::InstantCommand([] {});
}
