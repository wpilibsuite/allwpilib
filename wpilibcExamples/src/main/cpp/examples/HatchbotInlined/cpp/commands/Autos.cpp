// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/Autos.h"

#include <frc2/command/Commands.h>
#include <frc2/command/FunctionalCommand.h>

#include "Constants.h"

using namespace AutoConstants;

frc2::CommandPtr autos::SimpleAuto(DriveSubsystem* drive) {
  return frc2::FunctionalCommand(
             // Reset encoders on command start
             [drive] { drive->ResetEncoders(); },
             // Drive forward while the command is executing
             [drive] {
               drive->ArcadeDrive(AutoConstants::AUTO_DRIVE_SPEED, 0);
             },
             // Stop driving at the end of the command
             [drive](bool interrupted) { drive->ArcadeDrive(0, 0); },
             // End the command when the robot's driven distance exceeds the
             // desired value
             [drive] {
               return drive->GetAverageEncoderDistance() >=
                      AutoConstants::AUTO_DRIVE_DISTANCE_INCHES;
             },
             // Requires the drive subsystem
             {drive})
      .ToPtr();
}

frc2::CommandPtr autos::ComplexAuto(DriveSubsystem* drive,
                                    HatchSubsystem* hatch) {
  return frc2::cmd::Sequence(
      // Drive forward the specified distance
      frc2::FunctionalCommand(
          // Reset encoders on command start
          [drive] { drive->ResetEncoders(); },
          // Drive forward while the command is executing
          [drive] { drive->ArcadeDrive(AUTO_DRIVE_SPEED, 0); },
          // Stop driving at the end of the command
          [drive](bool interrupted) { drive->ArcadeDrive(0, 0); },
          // End the command when the robot's driven distance exceeds the
          // desired value
          [drive] {
            return drive->GetAverageEncoderDistance() >=
                   AUTO_DRIVE_DISTANCE_INCHES;
          },
          // Requires the drive subsystem
          {drive})
          .ToPtr(),
      // Release the hatch
      hatch->ReleaseHatchCommand(),
      // Drive backward the specified distance
      // Drive forward the specified distance
      frc2::FunctionalCommand(
          // Reset encoders on command start
          [drive] { drive->ResetEncoders(); },
          // Drive backward while the command is executing
          [drive] { drive->ArcadeDrive(-AUTO_DRIVE_SPEED, 0); },
          // Stop driving at the end of the command
          [drive](bool interrupted) { drive->ArcadeDrive(0, 0); },
          // End the command when the robot's driven distance exceeds the
          // desired value
          [drive] {
            return drive->GetAverageEncoderDistance() <=
                   AUTO_BACKUP_DISTANCE_INCHES;
          },
          // Requires the drive subsystem
          {drive})
          .ToPtr());
}
