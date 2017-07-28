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
 * REV Robotics Speed Controller
 */
class Spark : public PWMSpeedController {
 public:
  explicit Spark(int channel);
  virtual ~Spark() = default;
};

}  // namespace frc
