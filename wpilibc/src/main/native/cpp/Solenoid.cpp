// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Solenoid.h"

#include <utility>

#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>
#include <hal/Solenoid.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

Solenoid::Solenoid(int channel)
    : Solenoid(SensorUtil::GetDefaultSolenoidModule(), channel) {}

Solenoid::Solenoid(int moduleNumber, int channel)
    : SolenoidBase(moduleNumber), m_channel(channel) {
  if (!SensorUtil::CheckSolenoidModule(m_moduleNumber)) {
    throw FRC_MakeError(err::ModuleIndexOutOfRange, "Module {}",
                        m_moduleNumber);
  }
  if (!SensorUtil::CheckSolenoidChannel(m_channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", m_channel);
  }

  int32_t status = 0;
  m_solenoidHandle = HAL_InitializeSolenoidPort(
      HAL_GetPortWithModule(moduleNumber, channel), &status);
  FRC_CheckErrorStatus(status, "Module {} Channel {}", m_moduleNumber,
                       m_channel);

  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_channel + 1,
             m_moduleNumber + 1);
  SendableRegistry::GetInstance().AddLW(this, "Solenoid", m_moduleNumber,
                                        m_channel);
}

Solenoid::~Solenoid() {
  HAL_FreeSolenoidPort(m_solenoidHandle);
}

void Solenoid::Set(bool on) {
  int32_t status = 0;
  HAL_SetSolenoid(m_solenoidHandle, on, &status);
  FRC_CheckErrorStatus(status, "Module {} Channel {}", m_moduleNumber,
                       m_channel);
}

bool Solenoid::Get() const {
  int32_t status = 0;
  bool value = HAL_GetSolenoid(m_solenoidHandle, &status);
  FRC_CheckErrorStatus(status, "Module {} Channel {}", m_moduleNumber,
                       m_channel);

  return value;
}

void Solenoid::Toggle() {
  Set(!Get());
}

int Solenoid::GetChannel() const {
  return m_channel;
}

bool Solenoid::IsDisabledListed() const {
  int value = GetPCMSolenoidDisabledList(m_moduleNumber) & (1 << m_channel);
  return (value != 0);
}

void Solenoid::SetPulseDuration(units::second_t duration) {
  int32_t status = 0;
  HAL_SetOneShotDuration(m_solenoidHandle,
                         units::millisecond_t{duration}.to<int32_t>(), &status);
  FRC_CheckErrorStatus(status, "Module {} Channel {}", m_moduleNumber,
                       m_channel);
}

void Solenoid::StartPulse() {
  int32_t status = 0;
  HAL_FireOneShot(m_solenoidHandle, &status);
  FRC_CheckErrorStatus(status, "Module {} Channel {}", m_moduleNumber,
                       m_channel);
}

void Solenoid::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Solenoid");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { Set(false); });
  builder.AddBooleanProperty(
      "Value", [=]() { return Get(); }, [=](bool value) { Set(value); });
}
