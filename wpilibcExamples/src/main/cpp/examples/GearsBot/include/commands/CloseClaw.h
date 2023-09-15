// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command2/Command.h>
#include <frc/command2/CommandHelper.h>

#include "subsystems/Claw.h"

/**
 * Closes the claw until the limit switch is tripped.
 */
class CloseClaw : public frc::CommandHelper<frc::Command, CloseClaw> {
 public:
  explicit CloseClaw(Claw& claw);
  void Initialize() override;
  bool IsFinished() override;
  void End(bool interrupted) override;

 private:
  Claw* m_claw;
};
