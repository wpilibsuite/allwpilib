// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command/CommandHelper.h>
#include <frc/command/WaitCommand.h>

#include "subsystems/Claw.h"

/**
 * Opens the claw for one second. Real robots should use sensors, stalling
 * motors is BAD!
 */
class OpenClaw : public frc::CommandHelper<frc::WaitCommand, OpenClaw> {
 public:
  explicit OpenClaw(Claw& claw);
  void Initialize() override;
  void End(bool interrupted) override;

 private:
  Claw* m_claw;
};
