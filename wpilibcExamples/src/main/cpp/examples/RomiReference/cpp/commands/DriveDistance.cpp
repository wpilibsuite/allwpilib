// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveDistance.hpp"

#include "wpi/units/math.hpp"

void DriveDistance::Initialize() {
  drive->ArcadeDrive(0, 0);
  drive->ResetEncoders();
}

void DriveDistance::Execute() {
  drive->ArcadeDrive(velocity, 0);
}

void DriveDistance::End(bool interrupted) {
  drive->ArcadeDrive(0, 0);
}

bool DriveDistance::IsFinished() {
  return wpi::units::math::abs(drive->GetAverageDistance()) >= distance;
}
