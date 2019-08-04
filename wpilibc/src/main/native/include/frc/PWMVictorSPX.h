/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
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
 */
class PWMVictorSPX : public PWMSpeedController {
 public:
  /**
   * Construct a PWMVictorSPX connected via PWM.
   *
   * @param channel The PWM channel that the PWMVictorSPX is attached to. 0-9
   *                are on-board, 10-19 are on the MXP port
   */
  explicit PWMVictorSPX(int channel);

  PWMVictorSPX(PWMVictorSPX&&) = default;
  PWMVictorSPX& operator=(PWMVictorSPX&&) = default;
};

}  // namespace frc
