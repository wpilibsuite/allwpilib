/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SafePWM.h"

using namespace frc;

/**
 * Constructor for a SafePWM object taking a channel number.
 *
 * @param channel The PWM channel number 0-9 are on-board, 10-19 are on the MXP
 *                port
 */
SafePWM::SafePWM(int channel) : PWM(channel) {
  m_safetyHelper = std::make_unique<MotorSafetyHelper>(this);
  m_safetyHelper->SetSafetyEnabled(false);
}

/**
 * Set the expiration time for the PWM object.
 *
 * @param timeout The timeout (in seconds) for this motor object
 */
void SafePWM::SetExpiration(double timeout) {
  m_safetyHelper->SetExpiration(timeout);
}

/**
 * Return the expiration time for the PWM object.
 *
 * @returns The expiration time value.
 */
double SafePWM::GetExpiration() const {
  return m_safetyHelper->GetExpiration();
}

/**
 * Check if the PWM object is currently alive or stopped due to a timeout.
 *
 * @return a bool value that is true if the motor has NOT timed out and should
 *         still be running.
 */
bool SafePWM::IsAlive() const { return m_safetyHelper->IsAlive(); }

/**
 * Stop the motor associated with this PWM object.
 *
 * This is called by the MotorSafetyHelper object when it has a timeout for this
 * PWM and needs to stop it from running.
 */
void SafePWM::StopMotor() { SetDisabled(); }

/**
 * Enable/disable motor safety for this device.
 *
 * Turn on and off the motor safety option for this PWM object.
 *
 * @param enabled True if motor safety is enforced for this object
 */
void SafePWM::SetSafetyEnabled(bool enabled) {
  m_safetyHelper->SetSafetyEnabled(enabled);
}

/**
 * Check if motor safety is enabled for this object.
 *
 * @returns True if motor safety is enforced for this object
 */
bool SafePWM::IsSafetyEnabled() const {
  return m_safetyHelper->IsSafetyEnabled();
}

void SafePWM::GetDescription(llvm::raw_ostream& desc) const {
  desc << "PWM " << GetChannel();
}

/**
 * Feed the MotorSafety timer when setting the speed.
 *
 * This method is called by the subclass motor whenever it updates its speed,
 * thereby reseting the timeout value.
 *
 * @param speed Value to pass to the PWM class
 */
void SafePWM::SetSpeed(double speed) {
  PWM::SetSpeed(speed);
  m_safetyHelper->Feed();
}
