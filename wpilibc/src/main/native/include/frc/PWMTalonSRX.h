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
 * Cross the Road Electronics (CTRE) Talon SRX Speed Controller with PWM
 * control.
 */
class PWMTalonSRX : public PWMSpeedController {
 public:
  /**
   * Construct a PWMTalonSRX connected via PWM.
   *
   * @param channel The PWM channel that the PWMTalonSRX is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit PWMTalonSRX(int channel);

  PWMTalonSRX(PWMTalonSRX&&) = default;
  PWMTalonSRX& operator=(PWMTalonSRX&&) = default;
};

}  // namespace frc
