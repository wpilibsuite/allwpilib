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
 * Mindsensors SD540 Speed Controller.
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
