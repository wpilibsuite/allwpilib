/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/SetElevatorSetpoint.h"

#include <cmath>

#include "Robot.h"

SetElevatorSetpoint::SetElevatorSetpoint(double setpoint)
    : frc::Command("SetElevatorSetpoint") {
  m_setpoint = setpoint;
  Requires(&Robot::elevator);
}

// Called just before this Command runs the first time
void SetElevatorSetpoint::Initialize() {
  Robot::elevator.SetSetpoint(m_setpoint);
  Robot::elevator.Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetElevatorSetpoint::IsFinished() { return Robot::elevator.OnTarget(); }
