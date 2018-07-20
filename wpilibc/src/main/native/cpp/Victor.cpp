/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Victor.h"

#include <hal/HAL.h>

using namespace frc;

Victor::Victor(int channel) : PWMSpeedController(channel) {
  /* Note that the Victor uses the following bounds for PWM values.  These
   * values were determined empirically and optimized for the Victor 888. These
   * values should work reasonably well for Victor 884 controllers as well but
   * if users experience issues such as asymmetric behaviour around the deadband
   * or inability to saturate the controller in either direction, calibration is
   * recommended. The calibration procedure can be found in the Victor 884 User
   * Manual available from IFI.
   *
   *   2.027ms = full "forward"
   *   1.525ms = the "high end" of the deadband range
   *   1.507ms = center of the deadband range (off)
   *   1.49ms = the "low end" of the deadband range
   *   1.026ms = full "reverse"
   */
  SetBounds(2.027, 1.525, 1.507, 1.49, 1.026);
  SetPeriodMultiplier(kPeriodMultiplier_2X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Victor, GetChannel());
  SetName("Victor", GetChannel());
}
