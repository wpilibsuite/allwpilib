/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Solenoid.h"

#include <utility>

#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>
#include <hal/Solenoid.h>

#include "frc/SensorUtil.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

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
    wpi_setHALErrorWithRange(status, 0, HAL_GetNumSolenoidChannels(), channel);
    m_solenoidHandle = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_channel + 1,
             m_moduleNumber + 1);
  SendableRegistry::GetInstance().AddLW(this, "Solenoid", m_moduleNumber,
                                        m_channel);
}

Solenoid::~Solenoid() { HAL_FreeSolenoidPort(m_solenoidHandle); }

void Solenoid::Set(bool on) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  HAL_SetSolenoid(m_solenoidHandle, on, &status);
  wpi_setHALError(status);
}

bool Solenoid::Get() const {
  if (StatusIsFatal()) return false;

  int32_t status = 0;
  bool value = HAL_GetSolenoid(m_solenoidHandle, &status);
  wpi_setHALError(status);

  return value;
}

void Solenoid::Toggle() { Set(!Get()); }

bool Solenoid::IsBlackListed() const {
  int value = GetPCMSolenoidBlackList(m_moduleNumber) & (1 << m_channel);
  return (value != 0);
}

void Solenoid::SetPulseDuration(double durationSeconds) {
  int32_t durationMS = durationSeconds * 1000;
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetOneShotDuration(m_solenoidHandle, durationMS, &status);
  wpi_setHALError(status);
}

void Solenoid::StartPulse() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_FireOneShot(m_solenoidHandle, &status);
  wpi_setHALError(status);
}

void Solenoid::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Solenoid");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { Set(false); });
  builder.AddBooleanProperty(
      "Value", [=]() { return Get(); }, [=](bool value) { Set(value); });
}
