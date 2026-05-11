// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SysIdRoutineBot.hpp"

#include "wpi/commands2/Commands.hpp"

SysIdRoutineBot::SysIdRoutineBot() {
  ConfigureBindings();
}

void SysIdRoutineBot::ConfigureBindings() {
  drive.SetDefaultCommand(drive.ArcadeDriveCommand(
      [this] { return -driverController.GetLeftY(); },
      [this] { return -driverController.GetRightX(); }));

  // Using bumpers as a modifier and combining it with the buttons so that we
  // can have both sets of bindings at once
  (driverController.SouthFace() && driverController.RightBumper())
      .WhileTrue(drive.SysIdQuasistatic(wpi::cmd::sysid::Direction::kForward));
  (driverController.EastFace() && driverController.RightBumper())
      .WhileTrue(drive.SysIdQuasistatic(wpi::cmd::sysid::Direction::kReverse));
  (driverController.WestFace() && driverController.RightBumper())
      .WhileTrue(drive.SysIdDynamic(wpi::cmd::sysid::Direction::kForward));
  (driverController.NorthFace() && driverController.RightBumper())
      .WhileTrue(drive.SysIdDynamic(wpi::cmd::sysid::Direction::kReverse));

  shooter.SetDefaultCommand(shooter.RunShooterCommand(
      [this] { return driverController.GetLeftTriggerAxis(); }));

  (driverController.SouthFace() && driverController.LeftBumper())
      .WhileTrue(
          shooter.SysIdQuasistatic(wpi::cmd::sysid::Direction::kForward));
  (driverController.EastFace() && driverController.LeftBumper())
      .WhileTrue(
          shooter.SysIdQuasistatic(wpi::cmd::sysid::Direction::kReverse));
  (driverController.WestFace() && driverController.LeftBumper())
      .WhileTrue(shooter.SysIdDynamic(wpi::cmd::sysid::Direction::kForward));
  (driverController.NorthFace() && driverController.LeftBumper())
      .WhileTrue(shooter.SysIdDynamic(wpi::cmd::sysid::Direction::kReverse));
}

wpi::cmd::CommandPtr SysIdRoutineBot::GetAutonomousCommand() {
  return drive.Run([] {});
}
