/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/CloseClaw.h"

#include "Robot.h"

CloseClaw::CloseClaw(Claw* claw) : m_claw(claw) {
  SetName("CloseClaw");
  AddRequirements({m_claw});
}

// Called just before this Command runs the first time
void CloseClaw::Initialize() { m_claw->Close(); }

// Make this return true when this Command no longer needs to run execute()
bool CloseClaw::IsFinished() { return m_claw->IsGripping(); }

// Called once after isFinished returns true
void CloseClaw::End(bool) {
// NOTE: Doesn't stop in simulation due to lower friction causing the can to
// fall out
// + there is no need to worry about stalling the motor or crushing the can.
#ifndef SIMULATION
  m_claw->Stop();
#endif
}
