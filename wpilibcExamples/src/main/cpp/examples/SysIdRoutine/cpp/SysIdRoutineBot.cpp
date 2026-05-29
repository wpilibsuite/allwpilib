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
  (driverController.FaceDown() && driverController.RightBumper())
      .WhileTrue(drive.SysIdQuasistatic(wpi::cmd::sysid::Direction::kForward));
  (driverController.FaceRight() && driverController.RightBumper())
      .WhileTrue(drive.SysIdQuasistatic(wpi::cmd::sysid::Direction::kReverse));
  (driverController.FaceLeft() && driverController.RightBumper())
      .WhileTrue(drive.SysIdDynamic(wpi::cmd::sysid::Direction::kForward));
  (driverController.FaceUp() && driverController.RightBumper())
      .WhileTrue(drive.SysIdDynamic(wpi::cmd::sysid::Direction::kReverse));

  shooter.SetDefaultCommand(shooter.RunShooterCommand(
      [this] { return driverController.GetLeftTriggerAxis(); }));

  (driverController.FaceDown() && driverController.LeftBumper())
      .WhileTrue(
          shooter.SysIdQuasistatic(wpi::cmd::sysid::Direction::kForward));
  (driverController.FaceRight() && driverController.LeftBumper())
      .WhileTrue(
          shooter.SysIdQuasistatic(wpi::cmd::sysid::Direction::kReverse));
  (driverController.FaceLeft() && driverController.LeftBumper())
      .WhileTrue(shooter.SysIdDynamic(wpi::cmd::sysid::Direction::kForward));
  (driverController.FaceUp() && driverController.LeftBumper())
      .WhileTrue(shooter.SysIdDynamic(wpi::cmd::sysid::Direction::kReverse));
}

wpi::cmd::CommandPtr SysIdRoutineBot::GetAutonomousCommand() {
  return drive.Run([] {});
}
