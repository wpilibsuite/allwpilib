/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PWMSparkMax.h"

#include <hal/HAL.h>

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

PWMSparkMax::PWMSparkMax(int channel) : PWMSpeedController(channel) {
  /* Note that the SparkMax uses the following bounds for PWM values.
   *
   *   2.003ms = full "forward"
   *   1.55ms = the "high end" of the deadband range
   *   1.50ms = center of the deadband range (off)
   *   1.46ms = the "low end" of the deadband range
   *   0.999ms = full "reverse"
   */
  SetBounds(2.003, 1.55, 1.50, 1.46, .999);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_RevSparkMaxPWM, GetChannel());
  SendableRegistry::GetInstance().SetName(this, "PWMSparkMax", GetChannel());
}
