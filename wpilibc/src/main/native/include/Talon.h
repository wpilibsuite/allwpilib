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
 * Cross the Road Electronics (CTRE) Talon and Talon SR Speed Controller
 */
class Talon : public PWMSpeedController {
 public:
  explicit Talon(int channel);
  virtual ~Talon() = default;
};

}  // namespace frc
