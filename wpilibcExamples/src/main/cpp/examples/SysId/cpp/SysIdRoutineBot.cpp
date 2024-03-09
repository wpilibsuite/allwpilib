// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SysIdRoutineBot.h"

#include <frc2/command/Commands.h>

SysIdRoutineBot::SysIdRoutineBot() {
  ConfigureBindings();
}

void SysIdRoutineBot::ConfigureBindings() {
  m_drive.SetDefaultCommand(m_drive.ArcadeDriveCommand(
      [this] { return -m_driverController.GetLeftY(); },
      [this] { return -m_driverController.GetRightX(); }));

  // Using bumpers as a modifier and combining it with the buttons so that we
  // can have both sets of bindings at once
  (m_driverController.A() && m_driverController.RightBumper())
      .WhileTrue(m_drive.SysIdQuasistatic(frc2::sysid::Direction::kForward));
  (m_driverController.B() && m_driverController.RightBumper())
      .WhileTrue(m_drive.SysIdQuasistatic(frc2::sysid::Direction::kReverse));
  (m_driverController.X() && m_driverController.RightBumper())
      .WhileTrue(m_drive.SysIdDynamic(frc2::sysid::Direction::kForward));
  (m_driverController.Y() && m_driverController.RightBumper())
      .WhileTrue(m_drive.SysIdDynamic(frc2::sysid::Direction::kReverse));

  m_shooter.SetDefaultCommand(m_shooter.RunShooterCommand(
      [this] { return m_driverController.GetLeftTriggerAxis(); }));

  (m_driverController.A() && m_driverController.LeftBumper())
      .WhileTrue(m_shooter.SysIdQuasistatic(frc2::sysid::Direction::kForward));
  (m_driverController.B() && m_driverController.LeftBumper())
      .WhileTrue(m_shooter.SysIdQuasistatic(frc2::sysid::Direction::kReverse));
  (m_driverController.X() && m_driverController.LeftBumper())
      .WhileTrue(m_shooter.SysIdDynamic(frc2::sysid::Direction::kForward));
  (m_driverController.Y() && m_driverController.LeftBumper())
      .WhileTrue(m_shooter.SysIdDynamic(frc2::sysid::Direction::kReverse));
}

frc2::CommandPtr SysIdRoutineBot::GetAutonomousCommand() {
  return m_drive.Run([] {});
}
