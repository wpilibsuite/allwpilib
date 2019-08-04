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
 * REV Robotics Speed Controller.
 */
class Spark : public PWMSpeedController {
 public:
  /**
   * Constructor for a Spark.
   *
   * @param channel The PWM channel that the Spark is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit Spark(int channel);

  Spark(Spark&&) = default;
  Spark& operator=(Spark&&) = default;
};

}  // namespace frc
