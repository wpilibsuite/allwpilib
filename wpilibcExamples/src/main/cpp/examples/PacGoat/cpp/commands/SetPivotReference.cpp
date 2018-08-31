/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/SetPivotReference.h"

#include "Robot.h"

SetPivotReference::SetPivotReference(double reference) {
  m_reference = reference;
  Requires(&Robot::pivot);
}

// Called just before this Command runs the first time
void SetPivotReference::Initialize() {
  Robot::pivot.Enable();
  Robot::pivot.SetReference(m_reference);
}

// Make this return true when this Command no longer needs to run execute()
bool SetPivotReference::IsFinished() { return Robot::pivot.AtReference(); }
