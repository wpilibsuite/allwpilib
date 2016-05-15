/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "PWMSpeedController.h"

/**
 * Cross the Road Electronics (CTRE) Talon and Talon SR Speed Controller
 */
class Talon : public PWMSpeedController {
 public:
  explicit Talon(uint32_t channel);
  virtual ~Talon() = default;
};
