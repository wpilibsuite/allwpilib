// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Claw.h"

#include <frc/smartdashboard/SmartDashboard.h>

Claw::Claw() {
  // Let's show everything on the LiveWindow
  SetName("Claw");
  AddChild("Motor", &m_motor);
}

void Claw::Open() {
  m_motor.Set(-1);
}

void Claw::Close() {
  m_motor.Set(1);
}

void Claw::Stop() {
  m_motor.Set(0);
}

bool Claw::IsGripping() {
  return m_contact.Get();
}

void Claw::Log() {
  frc::SmartDashboard::PutBoolean("Claw switch", IsGripping());
}

void Claw::Periodic() {
  Log();
}
