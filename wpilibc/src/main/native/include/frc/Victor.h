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
 * Vex Robotics Victor 888 Speed Controller.
 *
 * The Vex Robotics Victor 884 Speed Controller can also be used with this
 * class but may need to be calibrated per the Victor 884 user manual.
 */
class Victor : public PWMSpeedController {
 public:
  /**
   * Constructor for a Victor.
   *
   * @param channel The PWM channel number that the Victor is attached to. 0-9
   *                are on-board, 10-19 are on the MXP port
   */
  explicit Victor(int channel);

  Victor(Victor&&) = default;
  Victor& operator=(Victor&&) = default;
};

}  // namespace frc
