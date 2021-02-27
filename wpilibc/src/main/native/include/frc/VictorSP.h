// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/PWMSpeedController.h"

namespace frc {

/**
 * Vex Robotics Victor SP Speed Controller.
 *
 * Note that the Victor SP uses the following bounds for PWM values. These
 * values should work reasonably well for most controllers, but if users
 * experience issues such as asymmetric behavior around the deadband or
 * inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the Victor SP User
 * Manual available from Vex.
 *
 * \li 2.004ms = full "forward"
 * \li 1.520ms = the "high end" of the deadband range
 * \li 1.500ms = center of the deadband range (off)
 * \li 1.480ms = the "low end" of the deadband range
 * \li 0.997ms = full "reverse"
 */
class VictorSP : public PWMSpeedController {
 public:
  /**
   * Constructor for a Victor SP.
   *
   * @param channel The PWM channel that the VictorSP is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit VictorSP(int channel);

  VictorSP(VictorSP&&) = default;
  VictorSP& operator=(VictorSP&&) = default;
};

}  // namespace frc
