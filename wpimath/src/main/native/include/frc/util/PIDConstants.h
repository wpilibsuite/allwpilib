// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

/** A class to configure a PIDController with */
struct PIDConstants {
 public:
  /// Factor for "proportion" control.
  const double Kp = 0.0;
  /// Factor for "integral" control.
  const double Ki = 0.0;
  /// Factor for "derivative" control.
  const double Kd = 0.0;

  /**
   * Create a new PIDConstants object.
   *
   * @param Kp     The proportional coefficient. Must be >= 0.
   * @param Ki     The integral coefficient. Must be >= 0.
   * @param Kd     The derivative coefficient. Must be >= 0.
   */
  constexpr PIDConstants(double Kp, double Ki, double Kd);
};

}  // namespace frc
