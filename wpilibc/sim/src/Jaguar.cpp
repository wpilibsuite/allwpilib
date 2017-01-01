/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Jaguar.h"

#include "LiveWindow/LiveWindow.h"

using namespace frc;

/**
 * @param channel The PWM channel that the Jaguar is attached to.
 */
Jaguar::Jaguar(int channel) : SafePWM(channel) {
  /*
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
  SetRaw(m_centerPwm);

  LiveWindow::GetInstance()->AddActuator("Jaguar", GetChannel(), this);
}

/**
 * Set the PWM value.
 *
 * The PWM value is set using a range of -1.0 to 1.0, appropriately
 * scaling the value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 */
void Jaguar::Set(double speed) { SetSpeed(speed); }

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
double Jaguar::Get() const { return GetSpeed(); }

/**
 * Common interface for disabling a motor.
 */
void Jaguar::Disable() { SetRaw(kPwmDisabled); }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void Jaguar::PIDWrite(double output) { Set(output); }
