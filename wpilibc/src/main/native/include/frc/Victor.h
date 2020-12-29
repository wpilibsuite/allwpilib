// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/PWMSpeedController.h"

namespace frc {

/**
 * Vex Robotics Victor 888 Speed Controller.
 *
 * The Vex Robotics Victor 884 Speed Controller can also be used with this
 * class but may need to be calibrated per the Victor 884 user manual.
 *
 * Note that the Victor uses the following bounds for PWM values.  These
 * values were determined empirically and optimized for the Victor 888. These
 * values should work reasonably well for Victor 884 controllers as well but
 * if users experience issues such as asymmetric behavior around the deadband
 * or inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the Victor 884 User
 * Manual available from Vex.
 *
 * \li 2.027ms = full "forward"
 * \li 1.525ms = the "high end" of the deadband range
 * \li 1.507ms = center of the deadband range (off)
 * \li 1.490ms = the "low end" of the deadband range
 * \li 1.026ms = full "reverse"
 */
class Victor : public PWMSpeedController {
 public:
  /**
   * Constructor for a Victor.
   *
   * @param channel The PWM channel number that the Victor is attached to. 0-9
   *                are on-board, 10-19 are on the MXP port
   */
  explicit Victor(int channel);

  Victor(Victor&&) = default;
  Victor& operator=(Victor&&) = default;
};

}  // namespace frc
