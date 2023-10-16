// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/TurnTime.h"

void TurnTime::Initialize() {
  m_timer.Start();
  m_drive->ArcadeDrive(0, 0);
}

void TurnTime::Execute() {
  m_drive->ArcadeDrive(0, m_speed);
}

void TurnTime::End(bool interrupted) {
  m_drive->ArcadeDrive(0, 0);
  m_timer.Stop();
  m_timer.Reset();
}

bool TurnTime::IsFinished() {
  return m_timer.HasElapsed(m_duration);
}
