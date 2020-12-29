// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/SetWristSetpoint.h"

#include "Robot.h"

SetWristSetpoint::SetWristSetpoint(double setpoint, Wrist* wrist)
    : m_setpoint(setpoint), m_wrist(wrist) {
  SetName("SetWristSetpoint");
  AddRequirements({m_wrist});
}

// Called just before this Command runs the first time
void SetWristSetpoint::Initialize() {
  m_wrist->SetSetpoint(m_setpoint);
  m_wrist->Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetWristSetpoint::IsFinished() {
  return m_wrist->GetController().AtSetpoint();
}
