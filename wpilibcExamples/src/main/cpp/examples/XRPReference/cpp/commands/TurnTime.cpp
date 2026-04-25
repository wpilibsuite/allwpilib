// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/TurnTime.hpp"

void TurnTime::Initialize() {
  timer.Start();
  drive->ArcadeDrive(0, 0);
}

void TurnTime::Execute() {
  drive->ArcadeDrive(0, velocity);
}

void TurnTime::End(bool interrupted) {
  drive->ArcadeDrive(0, 0);
  timer.Stop();
  timer.Reset();
}

bool TurnTime::IsFinished() {
  return timer.HasElapsed(duration);
}
