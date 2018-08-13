/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/PIDBase.h"

namespace frc {

/**
 * Class implements a synchronous PID control loop.
 *
 * Provides a calculate method for the user to call at their desired update
 * rate.
 */
class SynchronousPID : public PIDBase {
 public:
  /**
   * Allocate a PID object with the given constants for P, I, and D.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   */
  SynchronousPID(double Kp, double Ki, double Kd, PIDSource& source,
                 PIDOutput& output);

  /**
   * Allocate a PID object with the given constants for P, I, and D.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kf     the feed forward term
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   */
  SynchronousPID(double Kp, double Ki, double Kd, double Kf, PIDSource& source,
                 PIDOutput& output);

  SynchronousPID(const SynchronousPID&) = delete;
  SynchronousPID& operator=(const SynchronousPID) = delete;

  /**
   * Read the input, calculate the output accordingly, and write to the output.
   */
  void Calculate() override;
};

}  // namespace frc
