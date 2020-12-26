// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/PWMSpeedController.h"

namespace frc {

/**
 * Cross the Road Electronics (CTRE) Talon FX Speed Controller with PWM
 * control.
 *
 * Note that the Talon FX uses the following bounds for PWM values. These
 * values should work reasonably well for most controllers, but if users
 * experience issues such as asymmetric behavior around the deadband or
 * inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the Talon FX User
 * Manual available from Cross The Road Electronics.
 *
 * \li 2.004ms = full "forward"
 * \li 1.520ms = the "high end" of the deadband range
 * \li 1.500ms = center of the deadband range (off)
 * \li 1.480ms = the "low end" of the deadband range
 * \li 0.997ms = full "reverse"
 */
class PWMTalonFX : public PWMSpeedController {
 public:
  /**
   * Construct a Talon FX connected via PWM.
   *
   * @param channel The PWM channel that the Talon FX is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit PWMTalonFX(int channel);

  PWMTalonFX(PWMTalonFX&&) = default;
  PWMTalonFX& operator=(PWMTalonFX&&) = default;
};

}  // namespace frc
