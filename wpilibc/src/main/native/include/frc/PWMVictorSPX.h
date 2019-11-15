/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/PWMSpeedController.h"

namespace frc {

/**
 * Cross the Road Electronics (CTRE) Victor SPX Speed Controller with PWM
 * control.
 *
 * Note that the Victor SPX uses the following bounds for PWM values. These
 * values should work reasonably well for most controllers, but if users
 * experience issues such as asymmetric behavior around the deadband or
 * inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the Victor SPX User
 * Manual available from Cross The Road Electronics.
 *
 * \li 2.004ms = full "forward"
 * \li 1.520ms = the "high end" of the deadband range
 * \li 1.500ms = center of the deadband range (off)
 * \li 1.480ms = the "low end" of the deadband range
 * \li 0.997ms = full "reverse"
 */
class PWMVictorSPX : public PWMSpeedController {
 public:
  /**
   * Construct a Victor SPX connected via PWM.
   *
   * @param channel The PWM channel that the Victor SPX is attached to. 0-9
   *                are on-board, 10-19 are on the MXP port
   */
  explicit PWMVictorSPX(int channel);

  PWMVictorSPX(PWMVictorSPX&&) = default;
  PWMVictorSPX& operator=(PWMVictorSPX&&) = default;
};

}  // namespace frc
