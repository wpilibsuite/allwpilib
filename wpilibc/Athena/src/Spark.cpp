/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Spark.h"

#include "LiveWindow/LiveWindow.h"

/**
 * Note that the Spark uses the following bounds for PWM values. These values
 * should work reasonably well for
 * most controllers, but if users experience issues such as asymmetric behavior
 * around
 * the deadband or inability to saturate the controller in either direction,
 * calibration is recommended.
 * The calibration procedure can be found in the Spark User Manual available
 * from REV Robotics.
 *
 *   2.003ms = full "forward"
 *   1.55ms = the "high end" of the deadband range
 *   1.50ms = center of the deadband range (off)
 *   1.46ms = the "low end" of the deadband range
 *   0.999ms = full "reverse"
 */

/**
 * Constructor for a Spark
 * @param channel The PWM channel that the Spark is attached to. 0-9 are
 * on-board, 10-19 are on the MXP port
 */
Spark::Spark(uint32_t channel) : SafePWM(channel) {
  SetBounds(2.003, 1.55, 1.50, 1.46, .999);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetRaw(m_centerPwm);
  SetZeroLatch();

  HALReport(HALUsageReporting::kResourceType_RevSPARK, GetChannel());
  LiveWindow::GetInstance()->AddActuator("Spark", GetChannel(), this);
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
void Spark::Set(float speed, uint8_t syncGroup) {
  SetSpeed(m_isInverted ? -speed : speed);
}

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
float Spark::Get() const { return GetSpeed(); }

/**
 * Common interface for inverting direction of a speed controller.
 * @param isInverted The state of inversion, true is inverted.
 */
void Spark::SetInverted(bool isInverted) { m_isInverted = isInverted; }

/**
 * Common interface for the inverting direction of a speed controller.
 *
 * @return isInverted The state of inversion, true is inverted.
 *
 */
bool Spark::GetInverted() const { return m_isInverted; }

/**
 * Common interface for disabling a motor.
 */
void Spark::Disable() { SetRaw(kPwmDisabled); }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void Spark::PIDWrite(float output) { Set(output); }

/**
 * Common interface to stop the motor until Set is called again.
 */
void Spark::StopMotor() { this->SafePWM::StopMotor(); }