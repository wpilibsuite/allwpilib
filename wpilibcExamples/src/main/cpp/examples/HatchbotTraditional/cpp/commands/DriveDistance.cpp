// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveDistance.hpp"

#include <cmath>

DriveDistance::DriveDistance(double inches, double velocity,
                             DriveSubsystem* subsystem)
    : drive(subsystem), distance(inches), velocity(velocity) {
  AddRequirements(subsystem);
}

void DriveDistance::Initialize() {
  drive->ResetEncoders();
  drive->ArcadeDrive(velocity, 0);
}

void DriveDistance::Execute() {
  drive->ArcadeDrive(velocity, 0);
}

void DriveDistance::End(bool interrupted) {
  drive->ArcadeDrive(0, 0);
}

bool DriveDistance::IsFinished() {
  return std::abs(drive->GetAverageEncoderDistance()) >= distance;
}
