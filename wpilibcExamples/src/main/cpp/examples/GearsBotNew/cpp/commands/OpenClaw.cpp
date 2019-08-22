/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/OpenClaw.h"

#include "Robot.h"

OpenClaw::OpenClaw(Claw* claw) : frc2::WaitCommand(1), m_claw(claw) {
  SetName("OpenClaw");
  AddRequirements({m_claw});
}

// Called just before this Command runs the first time
void OpenClaw::Initialize() { Robot::claw.Open(); }

// Called once after isFinished returns true
void OpenClaw::End(bool) { Robot::claw.Stop(); }
