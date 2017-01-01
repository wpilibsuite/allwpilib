/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PWMSpeedController.h"

using namespace frc;

/**
 * Constructor for a PWM Speed Controller connected via PWM.
 *
 * @param channel The PWM channel that the controller is attached to. 0-9 are
 *                on-board, 10-19 are on the MXP port
 */
PWMSpeedController::PWMSpeedController(int channel) : SafePWM(channel) {}

/**
 * Set the PWM value.
 *
 * The PWM value is set using a range of -1.0 to 1.0, appropriately
 * scaling the value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 */
void PWMSpeedController::Set(double speed) {
  SetSpeed(m_isInverted ? -speed : speed);
}

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
double PWMSpeedController::Get() const { return GetSpeed(); }

/**
 * Common interface for disabling a motor.
 */
void PWMSpeedController::Disable() { SetDisabled(); }

/**
 * Common interface for inverting direction of a speed controller.
 *
 * @param isInverted The state of inversion, true is inverted.
 */
void PWMSpeedController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

/**
 * Common interface for the inverting direction of a speed controller.
 *
 * @return isInverted The state of inversion, true is inverted.
 *
 */
bool PWMSpeedController::GetInverted() const { return m_isInverted; }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void PWMSpeedController::PIDWrite(double output) { Set(output); }

/**
 * Common interface to stop the motor until Set is called again.
 */
void PWMSpeedController::StopMotor() { this->SafePWM::StopMotor(); }
