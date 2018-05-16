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
 * Mindsensors SD540 Speed Controller.
 */
class SD540 : public PWMSpeedController {
 public:
  explicit SD540(int channel);
};

}  // namespace frc
