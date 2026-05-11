// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RapidReactCommandBot.hpp"

#include "Constants.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/button/Trigger.hpp"

void RapidReactCommandBot::ConfigureBindings() {
  // Automatically run the storage motor whenever the ball storage is not full,
  // and turn it off whenever it fills. Uses subsystem-hosted trigger to
  // improve readability and make inter-subsystem communication easier.
  storage.HasCargo.WhileFalse(storage.RunCommand());

  // Automatically disable and retract the intake whenever the ball storage is
  // full.
  storage.HasCargo.OnTrue(intake.RetractCommand());

  // Control the drive with split-stick arcade controls
  drive.SetDefaultCommand(drive.ArcadeDriveCommand(
      [this] { return -driverController.GetLeftY(); },
      [this] { return -driverController.GetRightX(); }));

  // Deploy the intake with the West Face button
  driverController.WestFace().OnTrue(intake.IntakeCommand());
  // Retract the intake with the North Face button
  driverController.NorthFace().OnTrue(intake.RetractCommand());

  // Fire the shooter with the South Face button
  driverController.SouthFace().OnTrue(
      wpi::cmd::Parallel(shooter.ShootCommand(ShooterConstants::kShooterTarget),
                         storage.RunCommand())
          // Since we composed this inline we should give it a name
          .WithName("Shoot"));

  // Toggle compressor with the Start button
  driverController.Start().ToggleOnTrue(pneumatics.DisableCompressorCommand());
}

wpi::cmd::CommandPtr RapidReactCommandBot::GetAutonomousCommand() {
  return drive
      .DriveDistanceCommand(AutoConstants::kDriveDistance,
                            AutoConstants::kDriveVelocity)
      .WithTimeout(AutoConstants::kTimeout);
}
