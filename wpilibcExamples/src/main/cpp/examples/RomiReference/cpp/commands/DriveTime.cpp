// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveTime.hpp"

void DriveTime::Initialize() {
  timer.Start();
  drive->ArcadeDrive(0, 0);
}

void DriveTime::Execute() {
  drive->ArcadeDrive(velocity, 0);
}

void DriveTime::End(bool interrupted) {
  drive->ArcadeDrive(0, 0);
  timer.Stop();
  timer.Reset();
}

bool DriveTime::IsFinished() {
  return timer.HasElapsed(duration);
}
