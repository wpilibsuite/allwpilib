/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/PWMSpeedController.h"

namespace frc {

/**
 * Luminary Micro / Vex Robotics Jaguar Speed Controller with PWM control.
 *
 * Note that the Jaguar uses the following bounds for PWM values. These values
 * should work reasonably well for most controllers, but if users experience
 * issues such as asymmetric behavior around the deadband or inability to
 * saturate the controller in either direction, calibration is recommended. The
 * calibration procedure can be found in the Jaguar User Manual available from
 * Vex.
 *
 * \li 2.310ms = full "forward"
 * \li 1.550ms = the "high end" of the deadband range
 * \li 1.507ms = center of the deadband range (off)
 * \li 1.454ms = the "low end" of the deadband range
 * \li 0.697ms = full "reverse"
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
