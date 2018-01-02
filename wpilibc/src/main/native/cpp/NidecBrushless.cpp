/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NidecBrushless.h"

#include <HAL/HAL.h>

#include "SmartDashboard/SendableBuilder.h"

using namespace frc;

/**
 * Constructor.
 *
 * @param pwmChannel The PWM channel that the Nidec Brushless controller is
 *                   attached to. 0-9 are on-board, 10-19 are on the MXP port.
 * @param dioChannel The DIO channel that the Nidec Brushless controller is
 *                   attached to. 0-9 are on-board, 10-25 are on the MXP port.
 */
NidecBrushless::NidecBrushless(int pwmChannel, int dioChannel)
    : m_safetyHelper(this), m_dio(dioChannel), m_pwm(pwmChannel) {
  AddChild(&m_dio);
  AddChild(&m_pwm);
  m_safetyHelper.SetExpiration(0.0);
  m_safetyHelper.SetSafetyEnabled(false);

  // the dio controls the output (in PWM mode)
  m_dio.SetPWMRate(15625);
  m_dio.EnablePWM(0.5);

  HAL_Report(HALUsageReporting::kResourceType_NidecBrushless, pwmChannel);
  SetName("Nidec Brushless", pwmChannel);
}

/**
 * Set the PWM value.
 *
 * The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the
 * value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 */
void NidecBrushless::Set(double speed) {
  if (!m_disabled) {
    m_speed = speed;
    m_dio.UpdateDutyCycle(0.5 + 0.5 * (m_isInverted ? -speed : speed));
    m_pwm.SetRaw(0xffff);
  }
  m_safetyHelper.Feed();
}

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
double NidecBrushless::Get() const { return m_speed; }

void NidecBrushless::SetInverted(bool isInverted) { m_isInverted = isInverted; }

bool NidecBrushless::GetInverted() const { return m_isInverted; }

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void NidecBrushless::PIDWrite(double output) { Set(output); }

/**
 * Set the safety expiration time.
 *
 * @param timeout The timeout (in seconds) for this motor object
 */
void NidecBrushless::SetExpiration(double timeout) {
  m_safetyHelper.SetExpiration(timeout);
}

/**
 * Return the safety expiration time.
 *
 * @return The expiration time value.
 */
double NidecBrushless::GetExpiration() const {
  return m_safetyHelper.GetExpiration();
}

/**
 * Check if the motor is currently alive or stopped due to a timeout.
 *
 * @return a bool value that is true if the motor has NOT timed out and should
 *         still be running.
 */
bool NidecBrushless::IsAlive() const { return m_safetyHelper.IsAlive(); }

/**
 * Stop the motor. This is called by the MotorSafetyHelper object when it has a
 * timeout for this PWM and needs to stop it from running. Calling Set() will
 * re-enable the motor.
 */
void NidecBrushless::StopMotor() {
  m_dio.UpdateDutyCycle(0.5);
  m_pwm.SetDisabled();
}

/**
 * Check if motor safety is enabled.
 *
 * @return True if motor safety is enforced for this object
 */
bool NidecBrushless::IsSafetyEnabled() const {
  return m_safetyHelper.IsSafetyEnabled();
}

void NidecBrushless::SetSafetyEnabled(bool enabled) {
  m_safetyHelper.SetSafetyEnabled(enabled);
}

void NidecBrushless::GetDescription(llvm::raw_ostream& desc) const {
  desc << "Nidec " << GetChannel();
}

/**
 * Disable the motor.  The Enable() function must be called to re-enable
 * the motor.
 */
void NidecBrushless::Disable() {
  m_disabled = true;
  m_dio.UpdateDutyCycle(0.5);
  m_pwm.SetDisabled();
}

/**
 * Re-enable the motor after Disable() has been called.  The Set()
 * function must be called to set a new motor speed.
 */
void NidecBrushless::Enable() { m_disabled = false; }

/**
 * Gets the channel number associated with the object.
 *
 * @return The channel number.
 */
int NidecBrushless::GetChannel() const { return m_pwm.GetChannel(); }

void NidecBrushless::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Nidec Brushless");
  builder.SetSafeState([=]() { StopMotor(); });
  builder.AddDoubleProperty("Value", [=]() { return Get(); },
                            [=](double value) { Set(value); });
}
