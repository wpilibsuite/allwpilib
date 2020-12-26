// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/SetPivotSetpoint.h"

#include "Robot.h"

SetPivotSetpoint::SetPivotSetpoint(double setpoint) {
  m_setpoint = setpoint;
  Requires(&Robot::pivot);
}

// Called just before this Command runs the first time
void SetPivotSetpoint::Initialize() {
  Robot::pivot.Enable();
  Robot::pivot.SetSetpoint(m_setpoint);
}

// Make this return true when this Command no longer needs to run execute()
bool SetPivotSetpoint::IsFinished() { return Robot::pivot.OnTarget(); }
