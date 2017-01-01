/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Jaguar.h"

#include "HAL/HAL.h"
#include "LiveWindow/LiveWindow.h"

using namespace frc;

/**
 * Constructor for a Jaguar connected via PWM.
 *
 * @param channel The PWM channel that the Jaguar is attached to. 0-9 are
 *                on-board, 10-19 are on the MXP port
 */
Jaguar::Jaguar(int channel) : PWMSpeedController(channel) {
  /**
   * Input profile defined by Luminary Micro.
   *
   * Full reverse ranges from 0.671325ms to 0.6972211ms
   * Proportional reverse ranges from 0.6972211ms to 1.4482078ms
   * Neutral ranges from 1.4482078ms to 1.5517922ms
   * Proportional forward ranges from 1.5517922ms to 2.3027789ms
   * Full forward ranges from 2.3027789ms to 2.328675ms
   */
  SetBounds(2.31, 1.55, 1.507, 1.454, .697);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Jaguar, GetChannel());
  LiveWindow::GetInstance()->AddActuator("Jaguar", GetChannel(), this);
}
