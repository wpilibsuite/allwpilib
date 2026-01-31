// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SysIdRoutineBot.hpp"

#include "wpi/commands2/Commands.hpp"

SysIdRoutineBot::SysIdRoutineBot() {
  ConfigureBindings();
}

void SysIdRoutineBot::ConfigureBindings() {
  m_drive.SetDefaultCommand(m_drive.ArcadeDriveCommand(
      [this] { return -m_driverController.GetLeftY(); },
      [this] { return -m_driverController.GetRightX(); }));

  // Using bumpers as a modifier and combining it with the buttons so that we
  // can have both sets of bindings at once
  (m_driverController.SouthFace() && m_driverController.RightShoulder())
      .WhileTrue(
          m_drive.SysIdQuasistatic(wpi::cmd::sysid::Direction::kForward));
  (m_driverController.EastFace() && m_driverController.RightShoulder())
      .WhileTrue(
          m_drive.SysIdQuasistatic(wpi::cmd::sysid::Direction::kReverse));
  (m_driverController.WestFace() && m_driverController.RightShoulder())
      .WhileTrue(m_drive.SysIdDynamic(wpi::cmd::sysid::Direction::kForward));
  (m_driverController.NorthFace() && m_driverController.RightShoulder())
      .WhileTrue(m_drive.SysIdDynamic(wpi::cmd::sysid::Direction::kReverse));

  m_shooter.SetDefaultCommand(m_shooter.RunShooterCommand(
      [this] { return m_driverController.GetLeftTriggerAxis(); }));

  (m_driverController.SouthFace() && m_driverController.LeftShoulder())
      .WhileTrue(
          m_shooter.SysIdQuasistatic(wpi::cmd::sysid::Direction::kForward));
  (m_driverController.EastFace() && m_driverController.LeftShoulder())
      .WhileTrue(
          m_shooter.SysIdQuasistatic(wpi::cmd::sysid::Direction::kReverse));
  (m_driverController.WestFace() && m_driverController.LeftShoulder())
      .WhileTrue(m_shooter.SysIdDynamic(wpi::cmd::sysid::Direction::kForward));
  (m_driverController.NorthFace() && m_driverController.LeftShoulder())
      .WhileTrue(m_shooter.SysIdDynamic(wpi::cmd::sysid::Direction::kReverse));
}

wpi::cmd::CommandPtr SysIdRoutineBot::GetAutonomousCommand() {
  return m_drive.Run([] {});
}
