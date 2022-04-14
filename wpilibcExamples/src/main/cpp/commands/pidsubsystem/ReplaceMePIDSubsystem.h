// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/PIDSubsystem.h>

class ReplaceMePIDSubsystem : public frc::PIDSubsystem {
 public:
  ReplaceMePIDSubsystem();
  double ReturnPIDInput() override;
  void UsePIDOutput(double output) override;
  void InitDefaultCommand() override;
};
