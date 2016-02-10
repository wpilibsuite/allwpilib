/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Jaguar.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Constructor for a Jaguar connected via PWM
 * @param channel The PWM channel that the Jaguar is attached to. 0-9 are
 * on-board, 10-19 are on the MXP port
 */
Jaguar::Jaguar(uint32_t channel) : SafePWM(channel) {
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
  SetRaw(m_centerPwm);
  SetZeroLatch();

  HALReport(HALUsageReporting::kResourceType_Jaguar, GetChannel());
  LiveWindow::GetInstance()->AddActuator("Jaguar", GetChannel(), this);
}

/**
 * Set the PWM value.
 *
 * The PWM value is set using a range of -1.0 to 1.0, appropriately
 * scaling the value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 * @param syncGroup Unused interface.
 */
void Jaguar::Set(float speed, uint8_t syncGroup) {
  SetSpeed(m_isInverted ? -speed : speed);
}

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
float Jaguar::Get() const { return GetSpeed(); }

/**
 * Common interface for disabling a motor.
 */
void Jaguar::Disable() { SetRaw(kPwmDisabled); }

/**
* Common interface for inverting direction of a speed controller.
* @param isInverted The state of inversion, true is inverted.
*/
void Jaguar::SetInverted(bool isInverted) { m_isInverted = isInverted; }

/**
 * Common interface for the inverting direction of a speed controller.
 *
 * @return isInverted The state of inversion, true is inverted.
 *
 */
bool Jaguar::GetInverted() const { return m_isInverted; }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void Jaguar::PIDWrite(float output) { Set(output); }

/**
 * Common interface to stop the motor until Set is called again.
 */
void Jaguar::StopMotor() { this->SafePWM::StopMotor(); }