// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/pneumatic/DoubleSolenoid.hpp"

#include <utility>

#include "wpi/hal/Ports.h"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/NullDeleter.hpp"
#include "wpi/util/SensorUtil.hpp"

using namespace wpi;

DoubleSolenoid::DoubleSolenoid(int busId, int module,
                               PneumaticsModuleType moduleType,
                               int forwardChannel, int reverseChannel)
    : m_module{PneumaticsBase::GetForType(busId, module, moduleType)},
      m_forwardChannel{forwardChannel},
      m_reverseChannel{reverseChannel} {
  if (!m_module->CheckSolenoidChannel(m_forwardChannel)) {
    throw WPILIB_MakeError(err::ChannelIndexOutOfRange, "Channel {}",
                           m_forwardChannel);
  }
  if (!m_module->CheckSolenoidChannel(m_reverseChannel)) {
    throw WPILIB_MakeError(err::ChannelIndexOutOfRange, "Channel {}",
                           m_reverseChannel);
  }

  m_forwardMask = 1 << forwardChannel;
  m_reverseMask = 1 << reverseChannel;
  m_mask = m_forwardMask | m_reverseMask;

  int allocMask = m_module->CheckAndReserveSolenoids(m_mask);
  if (allocMask != 0) {
    if (allocMask == m_mask) {
      throw WPILIB_MakeError(err::ResourceAlreadyAllocated,
                             "Channels {} and {}", m_forwardChannel,
                             m_reverseChannel);
    } else if (allocMask == m_forwardMask) {
      throw WPILIB_MakeError(err::ResourceAlreadyAllocated, "Channel {}",
                             m_forwardChannel);
    } else {
      throw WPILIB_MakeError(err::ResourceAlreadyAllocated, "Channel {}",
                             m_reverseChannel);
    }
  }

  m_module->ReportUsage(
      fmt::format("Solenoid[{},{}]", m_forwardChannel, m_reverseChannel),
      "DoubleSolenoid");
}

DoubleSolenoid::DoubleSolenoid(int busId, PneumaticsModuleType moduleType,
                               int forwardChannel, int reverseChannel)
    : DoubleSolenoid{busId, PneumaticsBase::GetDefaultForType(moduleType),
                     moduleType, forwardChannel, reverseChannel} {}

DoubleSolenoid::~DoubleSolenoid() {
  if (m_module) {
    m_module->UnreserveSolenoids(m_mask);
  }
}

void DoubleSolenoid::Set(Value value) {
  int setValue = 0;

  switch (value) {
    case kOff:
      setValue = 0;
      break;
    case kForward:
      setValue = m_forwardMask;
      break;
    case kReverse:
      setValue = m_reverseMask;
      break;
  }

  m_module->SetSolenoids(m_mask, setValue);
}

DoubleSolenoid::Value DoubleSolenoid::Get() const {
  auto values = m_module->GetSolenoids();

  if ((values & m_forwardMask) != 0) {
    return Value::kForward;
  } else if ((values & m_reverseMask) != 0) {
    return Value::kReverse;
  } else {
    return Value::kOff;
  }
}

void DoubleSolenoid::Toggle() {
  Value value = Get();

  if (value == kForward) {
    Set(kReverse);
  } else if (value == kReverse) {
    Set(kForward);
  }
}

int DoubleSolenoid::GetFwdChannel() const {
  return m_forwardChannel;
}

int DoubleSolenoid::GetRevChannel() const {
  return m_reverseChannel;
}

bool DoubleSolenoid::IsFwdSolenoidDisabled() const {
  return (m_module->GetSolenoidDisabledList() & m_forwardMask) != 0;
}

bool DoubleSolenoid::IsRevSolenoidDisabled() const {
  return (m_module->GetSolenoidDisabledList() & m_reverseMask) != 0;
}

void DoubleSolenoid::LogTo(wpi::TelemetryTable& table) const {
  std::string_view str;
  switch (Get()) {
    case kForward:
      str = "Forward";
      break;
    case kReverse:
      str = "Reverse";
      break;
    default:
      str = "Off";
      break;
  }
  table.Log("Value", str);
}

std::string_view DoubleSolenoid::GetTelemetryType() const {
  return "Double Solenoid";
}
