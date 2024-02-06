// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/time.h"

namespace frc {
/** A class to configure a PIDController with */
class PIDConstants {
 public:
  /** P */
  double Kp;
  /** I */
  double Ki;
  /** D */
  double Kd;
  /** Integral Sum */
  double IZone;
  /** Period */
  units::second_t period;

  /* Create a new PIDConstants object
   *
   * @param Kp P
   * @param Ki I
   * @param Kd D
   * @param IZone Integral range
   */
  PIDConstants(double Kp, double Ki, double Kd, double IZone,
               units::second_t period);

  /* Create a new PIDConstants object with period set to a default of 20_ms
   *
   * @param Kp P
   * @param Ki I
   * @param Kd D
   * @param IZone Integral range
   */
  PIDConstants(double Kp, double Ki, double Kd, double IZone);

  /**
   * Create a new PIDConstants object with IZone set to a default 1 and period
   * set to a default 20_ms
   *
   * @param Kp P
   * @param Ki I
   * @param Kd D
   */
  PIDConstants(double Kp, double Ki, double Kd);
};
}  // namespace frc
