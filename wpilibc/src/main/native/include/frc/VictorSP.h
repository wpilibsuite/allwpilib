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
 * Vex Robotics Victor SP Speed Controller.
 */
class VictorSP : public PWMSpeedController {
 public:
  /**
   * Constructor for a VictorSP.
   *
   * @param channel The PWM channel that the VictorSP is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit VictorSP(int channel);

  VictorSP(VictorSP&&) = default;
  VictorSP& operator=(VictorSP&&) = default;
};

}  // namespace frc
