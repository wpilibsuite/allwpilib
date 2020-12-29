// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/PWMSpeedController.h"

namespace frc {

/**
 * Luminary Micro / Vex Robotics Jaguar Speed Controller with PWM control.
 *
 * Note that the Jaguar uses the following bounds for PWM values. These values
 * should work reasonably well for most controllers, but if users experience
 * issues such as asymmetric behavior around the deadband or inability to
 * saturate the controller in either direction, calibration is recommended. The
 * calibration procedure can be found in the Jaguar User Manual available from
 * Vex.
 *
 * \li 2.310ms = full "forward"
 * \li 1.550ms = the "high end" of the deadband range
 * \li 1.507ms = center of the deadband range (off)
 * \li 1.454ms = the "low end" of the deadband range
 * \li 0.697ms = full "reverse"
 */
class Jaguar : public PWMSpeedController {
 public:
  /**
   * Constructor for a Jaguar connected via PWM.
   *
   * @param channel The PWM channel that the Jaguar is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit Jaguar(int channel);

  Jaguar(Jaguar&&) = default;
  Jaguar& operator=(Jaguar&&) = default;
};

}  // namespace frc
