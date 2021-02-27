// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/Command.h>

/**
 * Moves the  pivot to a given angle. This command finishes when it is within
 * the tolerance, but leaves the PID loop running to maintain the position.
 * Other commands using the pivot should make sure they disable PID!
 */
class SetPivotSetpoint : public frc::Command {
 public:
  explicit SetPivotSetpoint(double setpoint);
  void Initialize() override;
  bool IsFinished() override;

 private:
  double m_setpoint;
};
