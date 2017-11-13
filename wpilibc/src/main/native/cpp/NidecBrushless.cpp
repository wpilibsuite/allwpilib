/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NidecBrushless.h"

#include <HAL/HAL.h>

#include "LiveWindow/LiveWindow.h"

using namespace frc;

/**
 * Constructor.
 *
 * @param pwmChannel The PWM channel that the Nidec Brushless controller is
 *                   attached to.  0-9 are on-board, 10-19 are on the MXP port
 * @param dioChannel The DIO channel that the Nidec Brushless controller is
 *                   attached to.  0-9 are on-board, 10-25 are on the MXP port
 */
NidecBrushless::NidecBrushless(int pwmChannel, int dioChannel)
    : m_safetyHelper(this), m_dio(dioChannel), m_pwm(pwmChannel) {
  m_safetyHelper.SetExpiration(0.0);
  m_safetyHelper.SetSafetyEnabled(false);

  // the dio controls the output (in PWM mode)
  m_dio.SetPWMRate(15625);
  m_dio.EnablePWM(0.5);

  // the pwm enables the controller
  m_pwm.SetRaw(0xffff);

  LiveWindow::GetInstance()->AddActuator("Nidec Brushless", pwmChannel, this);
  HAL_Report(HALUsageReporting::kResourceType_NidecBrushless, pwmChannel);
}

/**
 * Set the PWM value.
 *
 * <p>The PWM value is set using a range of -1.0 to 1.0, appropriately scaling
 * the value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 */
void NidecBrushless::Set(double speed) {
  m_speed = speed;
  m_dio.UpdateDutyCycle(0.5 + 0.5 * (m_isInverted ? -speed : speed));
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
 * still be running.
 */
bool NidecBrushless::IsAlive() const { return m_safetyHelper.IsAlive(); }

/**
 * Stop the motor. This is called by the MotorSafetyHelper object
 * when it has a timeout for this PWM and needs to stop it from running.
 */
void NidecBrushless::StopMotor() { Disable(); }

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

void NidecBrushless::Disable() { m_dio.UpdateDutyCycle(0.5); }

/**
 * Gets the channel number associated with the object.
 *
 * @return The channel number.
 */
int NidecBrushless::GetChannel() const { return m_pwm.GetChannel(); }

/*
 * Live Window code, only does anything if live window is activated.
 */
std::string NidecBrushless::GetSmartDashboardType() const {
  return "Nidec Brushless";
}

void NidecBrushless::InitTable(std::shared_ptr<nt::NetworkTable> subtable) {
  if (subtable) {
    m_valueEntry = subtable->GetEntry("Value");
    UpdateTable();
  } else {
    m_valueEntry = nt::NetworkTableEntry();
  }
}

void NidecBrushless::UpdateTable() {
  if (m_valueEntry) {
    m_valueEntry.SetDouble(Get());
  }
}

void NidecBrushless::StartLiveWindowMode() {
  Set(0);  // Stop for safety
  if (m_valueEntry) {
    m_valueListener = m_valueEntry.AddListener(
        [=](const nt::EntryNotification& event) {
          if (!event.value->IsDouble()) return;
          Set(event.value->GetDouble());
        },
        NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
  }
}

void NidecBrushless::StopLiveWindowMode() {
  Set(0);  // Stop for safety
  if (m_valueListener != 0) {
    m_valueEntry.RemoveListener(m_valueListener);
    m_valueListener = 0;
  }
}
