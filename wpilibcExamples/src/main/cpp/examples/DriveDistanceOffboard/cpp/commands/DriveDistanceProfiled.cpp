// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveDistanceProfiled.h"

#include "Constants.h"

using namespace DriveConstants;

DriveDistanceProfiled::DriveDistanceProfiled(units::meter_t distance,
                                             DriveSubsystem* drive)
    : CommandHelper{
          frc::TrapezoidProfile<units::meters>{
              // Limit the max acceleration and velocity
              {kMaxSpeed, kMaxAcceleration}},
          // Pipe the profile state to the drive
          [drive](auto setpointState) {
            drive->SetDriveStates(setpointState, setpointState);
          },
          // End at desired position in meters; implicitly starts at 0
          [distance] {
            return frc::TrapezoidProfile<units::meters>::State{distance, 0_mps};
          },
          [] { return frc::TrapezoidProfile<units::meters>::State{}; },
          // Require the drive
          {drive}} {
  // Reset drive encoders since we're starting at 0
  drive->ResetEncoders();
}
