/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "PWMSpeedController.h"

namespace frc {

/**
 * Cross the Road Electronics (CTRE) Victor SPX Speed Controller with PWM
 * control.
 */
class PWMVictorSPX : public PWMSpeedController {
 public:
  explicit PWMVictorSPX(int channel);
};

}  // namespace frc
