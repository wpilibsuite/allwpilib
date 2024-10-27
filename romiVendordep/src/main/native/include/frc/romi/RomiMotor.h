// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/motorcontrol/PWMMotorController.h>

namespace frc {

/**
 * @defgroup romi_api Romi Hardware API
 * @{
 */

/**
 * RomiMotor
 *
 * A general use PWM motor controller representing the motors on a Romi robot
 */
class RomiMotor : public PWMMotorController {
 public:
  /**
   * Constructor for a RomiMotor.
   *
   * @param channel The PWM channel that the RomiMotor is attached to.
   *                0 is left, 1 is right
   */
  explicit RomiMotor(int channel);

  RomiMotor(RomiMotor&&) = default;
  RomiMotor& operator=(RomiMotor&&) = default;
};

/** @} */

}  // namespace frc
