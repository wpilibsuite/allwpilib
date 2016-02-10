/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Victor.h"

#include "LiveWindow/LiveWindow.h"

/**
 * Constructor for a Victor
 * @param channel The PWM channel number that the Victor is attached to. 0-9 are
 * on-board, 10-19 are on the MXP port
 */
Victor::Victor(uint32_t channel) : SafePWM(channel) {
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
  SetRaw(m_centerPwm);
  SetZeroLatch();

  LiveWindow::GetInstance()->AddActuator("Victor", GetChannel(), this);
  HALReport(HALUsageReporting::kResourceType_Victor, GetChannel());
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
void Victor::Set(float speed, uint8_t syncGroup) {
  SetSpeed(m_isInverted ? -speed : speed);
}

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
float Victor::Get() const { return GetSpeed(); }

/**
 * Common interface for disabling a motor.
 */
void Victor::Disable() { SetRaw(kPwmDisabled); }
/**
* Common interface for inverting direction of a speed controller.
* @param isInverted The state of inversion, true is inverted.
*/
void Victor::SetInverted(bool isInverted) { m_isInverted = isInverted; }

/**
 * Common interface for the inverting direction of a speed controller.
 *
 * @return isInverted The state of inversion, true is inverted.
 *
 */
bool Victor::GetInverted() const { return m_isInverted; }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void Victor::PIDWrite(float output) { Set(output); }

/**
 * Common interface to stop the motor until Set is called again.
 */
void Victor::StopMotor() { this->SafePWM::StopMotor(); }