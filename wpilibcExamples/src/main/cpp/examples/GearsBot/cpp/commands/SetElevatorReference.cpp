/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/SetElevatorReference.h"

#include <cmath>

#include "Robot.h"

SetElevatorReference::SetElevatorReference(double reference)
    : frc::Command("SetElevatorReference") {
  m_reference = reference;
  Requires(&Robot::elevator);
}

// Called just before this Command runs the first time
void SetElevatorReference::Initialize() {
  Robot::elevator.SetReference(m_reference);
  Robot::elevator.Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetElevatorReference::IsFinished() {
  return Robot::elevator.AtReference();
}
