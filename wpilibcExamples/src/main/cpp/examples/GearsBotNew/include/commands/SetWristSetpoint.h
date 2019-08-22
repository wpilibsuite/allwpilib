/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "subsystems/Wrist.h"

#include <frc2/command/SendableCommandBase.h>
#include <frc2/command/CommandHelper.h>

/**
 * Move the wrist to a given angle. This command finishes when it is within
 * the tolerance, but leaves the PID loop running to maintain the position.
 * Other commands using the wrist should make sure they disable PID!
 */
class SetWristSetpoint : public frc2::CommandHelper<frc2::SendableCommandBase, SetWristSetpoint> {
 public:
  explicit SetWristSetpoint(double setpoint, Wrist* wrist);
  void Initialize() override;
  bool IsFinished() override;

 private:
  double m_setpoint;
  Wrist* m_wrist;
};
