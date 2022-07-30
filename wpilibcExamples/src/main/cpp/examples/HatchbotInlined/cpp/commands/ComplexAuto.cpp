// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/ComplexAuto.h"

#include <frc/command/FunctionalCommand.h>
#include <frc/command/InstantCommand.h>
#include <frc/command/ParallelRaceGroup.h>

using namespace AutoConstants;

ComplexAuto::ComplexAuto(DriveSubsystem* drive, HatchSubsystem* hatch) {
  AddCommands(
      // Drive forward the specified distance
      frc::FunctionalCommand(
          // Reset encoders on command start
          [&] { drive->ResetEncoders(); },
          // Drive forward while the command is executing
          [&] { drive->ArcadeDrive(kAutoDriveSpeed, 0); },
          // Stop driving at the end of the command
          [&](bool interrupted) { drive->ArcadeDrive(0, 0); },
          // End the command when the robot's driven distance exceeds the
          // desired value
          [&] {
            return drive->GetAverageEncoderDistance() >=
                   kAutoDriveDistanceInches;
          },
          // Requires the drive subsystem
          {drive}),
      // Release the hatch
      frc::InstantCommand([hatch] { hatch->ReleaseHatch(); }, {hatch}),
      // Drive backward the specified distance
      // Drive forward the specified distance
      frc::FunctionalCommand(
          // Reset encoders on command start
          [&] { drive->ResetEncoders(); },
          // Drive backward while the command is executing
          [&] { drive->ArcadeDrive(-kAutoDriveSpeed, 0); },
          // Stop driving at the end of the command
          [&](bool interrupted) { drive->ArcadeDrive(0, 0); },
          // End the command when the robot's driven distance exceeds the
          // desired value
          [&] {
            return drive->GetAverageEncoderDistance() <=
                   kAutoBackupDistanceInches;
          },
          // Requires the drive subsystem
          {drive}));
}
