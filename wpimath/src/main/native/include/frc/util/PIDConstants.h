// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {
/** A class to configure a PIDController with */
struct PIDConstants {
 public:
  /** P */
  double Kp;
  /** I */
  double Ki;
  /** D */
  double Kd;

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
