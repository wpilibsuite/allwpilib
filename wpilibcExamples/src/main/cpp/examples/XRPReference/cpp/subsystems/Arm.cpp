// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Arm.h"

void Arm::Periodic() {
  // This method will be called once per scheduler run.
}

void Arm::SetAngle(units::radian_t angle) {
  m_armServo.SetAngle(angle);
}
