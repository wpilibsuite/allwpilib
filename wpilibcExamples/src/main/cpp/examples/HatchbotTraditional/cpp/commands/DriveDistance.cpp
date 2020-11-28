/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

void DriveDistance::Execute() { m_drive->ArcadeDrive(m_speed, 0); }

void DriveDistance::End(bool interrupted) { m_drive->ArcadeDrive(0, 0); }

bool DriveDistance::IsFinished() {
  return std::abs(m_drive->GetAverageEncoderDistance()) >= m_distance;
}
