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
 * Cross the Road Electronics (CTRE) Talon and Talon SR Speed Controller.
 */
class Talon : public PWMSpeedController {
 public:
  /**
   * Constructor for a Talon (original or Talon SR).
   *
   * @param channel The PWM channel number that the Talon is attached to. 0-9
   *                are on-board, 10-19 are on the MXP port
   */
  explicit Talon(int channel);

  Talon(Talon&&) = default;
  Talon& operator=(Talon&&) = default;
};

}  // namespace frc
