// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/SetCollectionSpeed.h"

#include "Robot.h"

SetCollectionSpeed::SetCollectionSpeed(double speed) {
  Requires(&Robot::collector);
  m_speed = speed;
}

// Called just before this Command runs the first time
void SetCollectionSpeed::Initialize() {
  Robot::collector.SetSpeed(m_speed);
}
