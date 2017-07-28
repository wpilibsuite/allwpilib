/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "PWMSpeedController.h"

namespace frc {

/**
 * Cross the Road Electronics (CTRE) Talon SRX Speed Controller with PWM control
 */
class TalonSRX : public PWMSpeedController {
 public:
  explicit TalonSRX(int channel);
  virtual ~TalonSRX() = default;
};

}  // namespace frc
