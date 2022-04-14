// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveTime.h"

void DriveTime::Initialize() {
  m_timer.Start();
  m_drive->ArcadeDrive(0, 0);
}

void DriveTime::Execute() {
  m_drive->ArcadeDrive(m_speed, 0);
}

void DriveTime::End(bool interrupted) {
  m_drive->ArcadeDrive(0, 0);
  m_timer.Stop();
  m_timer.Reset();
}

bool DriveTime::IsFinished() {
  return m_timer.HasElapsed(m_duration);
}
