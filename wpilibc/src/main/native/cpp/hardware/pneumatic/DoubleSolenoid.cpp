// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/pneumatic/DoubleSolenoid.hpp"

#include <utility>

#include <wpi/hal/Ports.h>
#include <wpi/util/NullDeleter.hpp>
#include <wpi/util/SensorUtil.hpp>
#include <wpi/util/sendable/SendableBuilder.hpp>
#include <wpi/util/sendable/SendableRegistry.hpp>

#include "wpi/system/Errors.hpp"

using namespace wpi;

DoubleSolenoid::DoubleSolenoid(int busId, int module,
                               PneumaticsModuleType moduleType,
                               int forwardChannel, int reverseChannel)
    : m_module{PneumaticsBase::GetForType(busId, module, moduleType)},
      m_forwardChannel{forwardChannel},
      m_reverseChannel{reverseChannel} {
  if (!m_module->CheckSolenoidChannel(m_forwardChannel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}",
                        m_forwardChannel);
  }
  if (!m_module->CheckSolenoidChannel(m_reverseChannel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}",
                        m_reverseChannel);
  }

  m_forwardMask = 1 << forwardChannel;
  m_reverseMask = 1 << reverseChannel;
  m_mask = m_forwardMask | m_reverseMask;

  int allocMask = m_module->CheckAndReserveSolenoids(m_mask);
  if (allocMask != 0) {
    if (allocMask == m_mask) {
      throw FRC_MakeError(err::ResourceAlreadyAllocated, "Channels {} and {}",
                          m_forwardChannel, m_reverseChannel);
    } else if (allocMask == m_forwardMask) {
      throw FRC_MakeError(err::ResourceAlreadyAllocated, "Channel {}",
                          m_forwardChannel);
    } else {
      throw FRC_MakeError(err::ResourceAlreadyAllocated, "Channel {}",
                          m_reverseChannel);
    }
  }

  m_module->ReportUsage(
      fmt::format("Solenoid[{},{}]", m_forwardChannel, m_reverseChannel),
      "DoubleSolenoid");

  wpi::util::SendableRegistry::Add(this, "DoubleSolenoid",
                             m_module->GetModuleNumber(), m_forwardChannel);
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

void DoubleSolenoid::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Double Solenoid");
  builder.SetActuator(true);
  builder.AddSmallStringProperty(
      "Value",
      [=, this](wpi::util::SmallVectorImpl<char>& buf) -> std::string_view {
        switch (Get()) {
          case kForward:
            return "Forward";
          case kReverse:
            return "Reverse";
          default:
            return "Off";
        }
      },
      [=, this](std::string_view value) {
        Value lvalue = kOff;
        if (value == "Forward") {
          lvalue = kForward;
        } else if (value == "Reverse") {
          lvalue = kReverse;
        }
        Set(lvalue);
      });
}
