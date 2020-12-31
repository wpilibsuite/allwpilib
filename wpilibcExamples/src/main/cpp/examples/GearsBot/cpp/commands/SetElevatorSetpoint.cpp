// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/SetElevatorSetpoint.h"

#include <cmath>

#include "Robot.h"

SetElevatorSetpoint::SetElevatorSetpoint(double setpoint, Elevator* elevator)
    : m_setpoint(setpoint), m_elevator(elevator) {
  SetName("SetElevatorSetpoint");
  AddRequirements({m_elevator});
}

// Called just before this Command runs the first time
void SetElevatorSetpoint::Initialize() {
  m_elevator->SetSetpoint(m_setpoint);
  m_elevator->Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetElevatorSetpoint::IsFinished() {
  return m_elevator->GetController().AtSetpoint();
}
