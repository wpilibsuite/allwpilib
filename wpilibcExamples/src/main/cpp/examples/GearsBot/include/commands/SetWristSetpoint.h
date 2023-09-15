// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command2/Command.h>
#include <frc/command2/CommandHelper.h>

#include "subsystems/Wrist.h"

/**
 * Move the wrist to a given angle. This command finishes when it is within
 * the tolerance, but leaves the PID loop running to maintain the position.
 * Other commands using the wrist should make sure they disable PID!
 */
class SetWristSetpoint
    : public frc::CommandHelper<frc::Command, SetWristSetpoint> {
 public:
  explicit SetWristSetpoint(double setpoint, Wrist& wrist);
  void Initialize() override;
  bool IsFinished() override;

 private:
  double m_setpoint;
  Wrist* m_wrist;
};
