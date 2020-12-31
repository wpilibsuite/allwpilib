// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/PWMSpeedController.h"

namespace frc {

/**
 * Mindsensors SD540 Speed Controller.
 *
 * Note that the SD540 uses the following bounds for PWM values. These values
 * should work reasonably well for most controllers, but if users experience
 * issues such as asymmetric behavior around the deadband or inability to
 * saturate the controller in either direction, calibration is recommended.
 * The calibration procedure can be found in the SD540 User Manual available
 * from Mindsensors.
 *
 * \li 2.05ms = full "forward"
 * \li 1.55ms = the "high end" of the deadband range
 * \li 1.50ms = center of the deadband range (off)
 * \li 1.44ms = the "low end" of the deadband range
 * \li 0.94ms = full "reverse"
 */
class SD540 : public PWMSpeedController {
 public:
  /**
   * Constructor for a SD540.
   *
   * @param channel The PWM channel that the SD540 is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit SD540(int channel);

  SD540(SD540&&) = default;
  SD540& operator=(SD540&&) = default;
};

}  // namespace frc
