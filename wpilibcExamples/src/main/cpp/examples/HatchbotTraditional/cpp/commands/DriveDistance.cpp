// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveDistance.h"

#include <cmath>

DriveDistance::DriveDistance(double inches, double speed,
                             DriveSubsystem* subsystem)
    : m_drive(subsystem), m_distance(inches), m_speed(speed) {
  AddRequirements({subsystem});
}

void DriveDistance::Initialize() {
  m_drive->ResetEncoders();
  m_drive->ArcadeDrive(m_speed, 0);
}

void DriveDistance::Execute() {
  m_drive->ArcadeDrive(m_speed, 0);
}

void DriveDistance::End(bool interrupted) {
  m_drive->ArcadeDrive(0, 0);
}

bool DriveDistance::IsFinished() {
  return std::abs(m_drive->GetAverageEncoderDistance()) >= m_distance;
}
