/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
