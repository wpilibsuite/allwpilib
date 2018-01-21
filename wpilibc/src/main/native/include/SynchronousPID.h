/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "PIDBase.h"

namespace frc {

/**
 * Class implements a synchronous PID control loop.
 *
 * Provides a calculate method for the user to call at their desired update
 * rate.
 */
class SynchronousPID : public PIDBase {
 public:
  SynchronousPID(double Kp, double Ki, double Kd, PIDSource& source,
                 PIDOutput& output, double period = 0.05);
  SynchronousPID(double Kp, double Ki, double Kd, double Kf, PIDSource& source,
                 PIDOutput& output, double period = 0.05);

  SynchronousPID(const SynchronousPID&) = delete;
  SynchronousPID& operator=(const SynchronousPID) = delete;

  void Calculate() override;
};

}  // namespace frc
