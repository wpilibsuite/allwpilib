/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/SetWristReference.h"

#include "Robot.h"

SetWristReference::SetWristReference(double reference)
    : frc::Command("SetWristReference") {
  m_reference = reference;
  Requires(&Robot::wrist);
}

// Called just before this Command runs the first time
void SetWristReference::Initialize() {
  Robot::wrist.SetReference(m_reference);
  Robot::wrist.Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetWristReference::IsFinished() { return Robot::wrist.AtReference(); }
