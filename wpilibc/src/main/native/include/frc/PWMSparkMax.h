/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/PWMSpeedController.h"

namespace frc {

/**
 * REV Robotics SPARK MAX Speed Controller.
 *
 * Note that the SPARK MAX uses the following bounds for PWM values. These
 * values should work reasonably well for most controllers, but if users
 * experience issues such as asymmetric behavior around the deadband or
 * inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the SPARK MAX User
 * Manual available from REV Robotics.
 *
 * \li 2.003ms = full "forward"
 * \li 1.550ms = the "high end" of the deadband range
 * \li 1.500ms = center of the deadband range (off)
 * \li 1.460ms = the "low end" of the deadband range
 * \li 0.999ms = full "reverse"
 */
class PWMSparkMax : public PWMSpeedController {
 public:
  /**
   * Constructor for a SPARK MAX.
   *
   * @param channel The PWM channel that the SPARK MAX is attached to. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit PWMSparkMax(int channel);

  PWMSparkMax(PWMSparkMax&&) = default;
  PWMSparkMax& operator=(PWMSparkMax&&) = default;
};

}  // namespace frc
