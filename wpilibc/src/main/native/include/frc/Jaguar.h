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
 * Luminary Micro / Vex Robotics Jaguar Speed Controller with PWM control.
 */
class Jaguar : public PWMSpeedController {
 public:
  /**
   * Constructor for a Jaguar connected via PWM.
   *
   * @param channel The PWM channel that the Jaguar is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit Jaguar(int channel);

  Jaguar(Jaguar&&) = default;
  Jaguar& operator=(Jaguar&&) = default;
};

}  // namespace frc
