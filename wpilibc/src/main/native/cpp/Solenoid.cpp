/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Solenoid.h"

#include <utility>

#include <hal/HAL.h>
#include <hal/Ports.h>
#include <hal/Solenoid.h>

#include "frc/SensorUtil.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

Solenoid::Solenoid(int channel)
    : Solenoid(SensorUtil::GetDefaultSolenoidModule(), channel) {}

Solenoid::Solenoid(int moduleNumber, int channel)
    : SolenoidBase(moduleNumber), m_channel(channel) {
  if (!SensorUtil::CheckSolenoidModule(m_moduleNumber)) {
    wpi_setWPIErrorWithContext(ModuleIndexOutOfRange,
                               "Solenoid Module " + wpi::Twine(m_moduleNumber));
    return;
  }
  if (!SensorUtil::CheckSolenoidChannel(m_channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "Solenoid Channel " + wpi::Twine(m_channel));
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

Solenoid::~Solenoid() { HAL_FreeSolenoidPort(m_solenoidHandle); }

Solenoid::Solenoid(Solenoid&& rhs)
    : SolenoidBase(std::move(rhs)), m_channel(std::move(rhs.m_channel)) {
  std::swap(m_solenoidHandle, rhs.m_solenoidHandle);
}

Solenoid& Solenoid::operator=(Solenoid&& rhs) {
  SolenoidBase::operator=(std::move(rhs));

  std::swap(m_solenoidHandle, rhs.m_solenoidHandle);
  m_channel = std::move(rhs.m_channel);

  return *this;
}

void Solenoid::Set(bool on) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetSolenoid(m_solenoidHandle, on, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

bool Solenoid::Get() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetSolenoid(m_solenoidHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

bool Solenoid::IsBlackListed() const {
  int value = GetPCMSolenoidBlackList(m_moduleNumber) & (1 << m_channel);
  return (value != 0);
}

void Solenoid::SetPulseDuration(double durationSeconds) {
  int32_t durationMS = durationSeconds * 1000;
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetOneShotDuration(m_solenoidHandle, durationMS, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void Solenoid::StartPulse() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_FireOneShot(m_solenoidHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void Solenoid::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Solenoid");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { Set(false); });
  builder.AddBooleanProperty("Value", [=]() { return Get(); },
                             [=](bool value) { Set(value); });
}
