/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Solenoid.h"

#include <HAL/HAL.h>
#include <HAL/Ports.h>
#include <HAL/Solenoid.h>

#include "SensorBase.h"
#include "SmartDashboard/SendableBuilder.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Constructor using the default PCM ID (0).
 *
 * @param channel The channel on the PCM to control (0..7).
 */
Solenoid::Solenoid(int channel)
    : Solenoid(SensorBase::GetDefaultSolenoidModule(), channel) {}

/**
 * Constructor.
 *
 * @param moduleNumber The CAN ID of the PCM the solenoid is attached to
 * @param channel      The channel on the PCM to control (0..7).
 */
Solenoid::Solenoid(int moduleNumber, int channel)
    : SolenoidBase(moduleNumber), m_channel(channel) {
  if (!SensorBase::CheckSolenoidModule(m_moduleNumber)) {
    wpi_setWPIErrorWithContext(
        ModuleIndexOutOfRange,
        "Solenoid Module " + llvm::Twine(m_moduleNumber));
    return;
  }
  if (!SensorBase::CheckSolenoidChannel(m_channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "Solenoid Channel " + llvm::Twine(m_channel));
    return;
  }

  int32_t status = 0;
  m_solenoidHandle = HAL_InitializeSolenoidPort(
      HAL_GetPortWithModule(moduleNumber, channel), &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumSolenoidChannels(),
                                 channel, HAL_GetErrorMessage(status));
    m_solenoidHandle = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_channel,
             m_moduleNumber);
  SetName("Solenoid", m_moduleNumber, m_channel);
}

/**
 * Destructor.
 */
Solenoid::~Solenoid() { HAL_FreeSolenoidPort(m_solenoidHandle); }

/**
 * Set the value of a solenoid.
 *
 * @param on Turn the solenoid output off or on.
 */
void Solenoid::Set(bool on) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetSolenoid(m_solenoidHandle, on, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Read the current value of the solenoid.
 *
 * @return The current value of the solenoid.
 */
bool Solenoid::Get() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetSolenoid(m_solenoidHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Check if solenoid is blacklisted.
 *
 * If a solenoid is shorted, it is added to the blacklist and
 * disabled until power cycle, or until faults are cleared.
 *
 * @see ClearAllPCMStickyFaults()
 *
 * @return If solenoid is disabled due to short.
 */
bool Solenoid::IsBlackListed() const {
  int value = GetPCMSolenoidBlackList(m_moduleNumber) & (1 << m_channel);
  return (value != 0);
}

/**
 * Set the pulse duration in the PCM. This is used in conjunction with
 * the startPulse method to allow the PCM to control the timing of a pulse.
 * The timing can be controlled in 0.01 second increments.
 *
 * @param durationSeconds The duration of the pulse, from 0.01 to 2.55 seconds.
 *
 * @see startPulse()
 */
void Solenoid::SetPulseDuration(double durationSeconds) {
  int32_t durationMS = durationSeconds * 1000;
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetOneShotDuration(m_solenoidHandle, durationMS, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Trigger the PCM to generate a pulse of the duration set in
 * setPulseDuration.
 *
 * @see setPulseDuration()
 */
void Solenoid::StartPulse() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_FireOneShot(m_solenoidHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void Solenoid::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Solenoid");
  builder.SetSafeState([=]() { Set(false); });
  builder.AddBooleanProperty("Value", [=]() { return Get(); },
                             [=](bool value) { Set(value); });
}
