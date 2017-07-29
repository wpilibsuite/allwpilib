/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PWMSpeedController.h"

/**
 * Constructor for a PWM Speed Controller connected via PWM.
 *
 * @param channel The PWM channel that the controller is attached to. 0-9 are
 *                on-board, 10-19 are on the MXP port
 */
frc::PWMSpeedController::PWMSpeedController(int channel) : SafePWM(channel) {}

/**
 * Set the PWM value.
 *
 * The PWM value is set using a range of -1.0 to 1.0, appropriately
 * scaling the value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 */
void frc::PWMSpeedController::Set(double speed) {
  SetSpeed(m_isInverted ? -speed : speed);
}

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
double frc::PWMSpeedController::Get() const { return GetSpeed(); }

void frc::PWMSpeedController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool frc::PWMSpeedController::GetInverted() const { return m_isInverted; }

void frc::PWMSpeedController::Disable() { SetDisabled(); }

void frc::PWMSpeedController::StopMotor() { SafePWM::StopMotor(); }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void frc::PWMSpeedController::PIDWrite(double output) { Set(output); }
