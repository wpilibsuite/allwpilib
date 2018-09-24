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
 * Digilent DMC 60 Speed Controller.
 */
class DMC60 : public PWMSpeedController {
 public:
  /**
   * Constructor for a Digilent DMC 60.
   *
   * @param channel The PWM channel that the DMC 60 is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit DMC60(int channel);

  DMC60(DMC60&&) = default;
  DMC60& operator=(DMC60&&) = default;
};

}  // namespace frc
