/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>

#include "subsystems/Elevator.h"

/**
 * Move the elevator to a given location. This command finishes when it is
 * within
 * the tolerance, but leaves the PID loop running to maintain the position.
 * Other
 * commands using the elevator should make sure they disable PID!
 */
class SetElevatorSetpoint
    : public frc2::CommandHelper<frc2::CommandBase, SetElevatorSetpoint> {
 public:
  explicit SetElevatorSetpoint(double setpoint, Elevator* elevator);
  void Initialize() override;
  bool IsFinished() override;

 private:
  double m_setpoint;
  Elevator* m_elevator;
};
