// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include "commands/DriveDistanceProfiled.h"

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
      DriveDistanceProfiled(3_m, &m_drive).WithTimeout(10_s));

  // Do the same thing as above when the 'B' button is pressed, but defined
  // inline
  m_driverController.B().OnTrue(
      frc2::TrapezoidProfileCommand<units::meters>(
          frc::TrapezoidProfile<units::meters>(
              // Limit the max acceleration and velocity
              {DriveConstants::kMaxSpeed, DriveConstants::kMaxAcceleration},
              // End at desired position in meters; implicitly starts at 0
              {3_m, 0_mps}),
          // Pipe the profile state to the drive
          [this](auto setpointState) {
            m_drive.SetDriveStates(setpointState, setpointState);
          },
          // Require the drive
          {&m_drive})
          // Convert to CommandPtr
          .ToPtr()
          .BeforeStarting(
              frc2::cmd::RunOnce([this]() { m_drive.ResetEncoders(); }, {}))
          .WithTimeout(10_s));
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return nullptr;
}
