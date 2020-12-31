// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/OpenClaw.h"

#include "Robot.h"

OpenClaw::OpenClaw(Claw* claw)
    : frc2::CommandHelper<frc2::WaitCommand, OpenClaw>(1_s), m_claw(claw) {
  SetName("OpenClaw");
  AddRequirements({m_claw});
}

// Called just before this Command runs the first time
void OpenClaw::Initialize() {
  m_claw->Open();
  frc2::WaitCommand::Initialize();
}

// Called once after isFinished returns true
void OpenClaw::End(bool) {
  m_claw->Stop();
}
