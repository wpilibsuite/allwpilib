// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/shuffleboard/Shuffleboard.h>
#include <frc2/command/Commands.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/button/JoystickButton.h>
#include <units/angle.h>

namespace ac = AutoConstants;

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

  // Move the arm to 2 radians above horizontal when the 'A' button is pressed.
  m_driverController.A().OnTrue(frc2::cmd::RunOnce(
      [this] {
        m_arm.SetGoal(2_rad);
        m_arm.Enable();
      },
      {&m_arm}));

  // Move the arm to neutral position when the 'B' button is pressed.
  m_driverController.B().OnTrue(frc2::cmd::RunOnce(
      [this] {
        m_arm.SetGoal(ArmConstants::kArmOffset);
        m_arm.Enable();
      },
      {&m_arm}));

  // Disable the arm controller when Y is pressed.
  m_driverController.Y().OnTrue(
      frc2::cmd::RunOnce([this] { m_arm.Disable(); }, {&m_arm}));

  // While holding the shoulder button, drive at half speed
  m_driverController.RightBumper()
      .OnTrue(frc2::cmd::RunOnce([this] { m_drive.SetMaxOutput(0.5); }))
      .OnFalse(frc2::cmd::RunOnce([this] { m_drive.SetMaxOutput(1.0); }));
}

void RobotContainer::DisablePIDSubsystems() {
  m_arm.Disable();
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  return nullptr;
}
