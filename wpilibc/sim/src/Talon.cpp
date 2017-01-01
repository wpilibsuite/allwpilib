/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Talon.h"

#include "LiveWindow/LiveWindow.h"

using namespace frc;

/**
 * @param channel The PWM channel that the Talon is attached to.
 */
Talon::Talon(int channel) : SafePWM(channel) {
  /* Note that the Talon uses the following bounds for PWM values. These values
   * should work reasonably well for most controllers, but if users experience
   * issues such as asymmetric behavior around the deadband or inability to
   * saturate the controller in either direction, calibration is recommended.
   * The calibration procedure can be found in the Talon User Manual available
   * from CTRE.
   *
   *   - 211 = full "forward"
   *   - 133 = the "high end" of the deadband range
   *   - 129 = center of the deadband range (off)
   *   - 125 = the "low end" of the deadband range
   *   - 49 = full "reverse"
   */
  SetBounds(2.037, 1.539, 1.513, 1.487, .989);
  SetPeriodMultiplier(kPeriodMultiplier_2X);
  SetRaw(m_centerPwm);

  LiveWindow::GetInstance()->AddActuator("Talon", GetChannel(), this);
}

/**
 * Set the PWM value.
 *
 * The PWM value is set using a range of -1.0 to 1.0, appropriately
 * scaling the value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 */
void Talon::Set(double speed) { SetSpeed(speed); }

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
double Talon::Get() const { return GetSpeed(); }

/**
 * Common interface for disabling a motor.
 */
void Talon::Disable() { SetRaw(kPwmDisabled); }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void Talon::PIDWrite(double output) { Set(output); }
