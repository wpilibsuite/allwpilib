/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Talon.h"

#include <hal/HAL.h>

using namespace frc;

Talon::Talon(int channel) : PWMSpeedController(channel) {
  /* Note that the Talon uses the following bounds for PWM values. These values
   * should work reasonably well for most controllers, but if users experience
   * issues such as asymmetric behavior around the deadband or inability to
   * saturate the controller in either direction, calibration is recommended.
   * The calibration procedure can be found in the Talon User Manual available
   * from CTRE.
   *
   *   2.037ms = full "forward"
   *   1.539ms = the "high end" of the deadband range
   *   1.513ms = center of the deadband range (off)
   *   1.487ms = the "low end" of the deadband range
   *   0.989ms = full "reverse"
   */
  SetBounds(2.037, 1.539, 1.513, 1.487, .989);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Talon, GetChannel());
  SetName("Talon", GetChannel());
}
