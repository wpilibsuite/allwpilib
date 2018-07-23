/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SD540.h"

#include <hal/HAL.h>

using namespace frc;

SD540::SD540(int channel) : PWMSpeedController(channel) {
  /* Note that the SD540 uses the following bounds for PWM values. These values
   * should work reasonably well for most controllers, but if users experience
   * issues such as asymmetric behavior around the deadband or inability to
   * saturate the controller in either direction, calibration is recommended.
   * The calibration procedure can be found in the SD540 User Manual available
   * from Mindsensors.
   *
   *   2.05ms = full "forward"
   *   1.55ms = the "high end" of the deadband range
   *   1.50ms = center of the deadband range (off)
   *   1.44ms = the "low end" of the deadband range
   *   0.94ms = full "reverse"
   */
  SetBounds(2.05, 1.55, 1.50, 1.44, .94);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_MindsensorsSD540, GetChannel());
  SetName("SD540", GetChannel());
}
