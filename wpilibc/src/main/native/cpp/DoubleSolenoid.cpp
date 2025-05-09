// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DoubleSolenoid.h"

#include <utility>

#include <hal/Ports.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

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

  wpi::SendableRegistry::Add(this, "DoubleSolenoid",
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
    case OFF:
      setValue = 0;
      break;
    case FORWARD:
      setValue = m_forwardMask;
      break;
    case REVERSE:
      setValue = m_reverseMask;
      break;
  }

  m_module->SetSolenoids(m_mask, setValue);
}

DoubleSolenoid::Value DoubleSolenoid::Get() const {
  auto values = m_module->GetSolenoids();

  if ((values & m_forwardMask) != 0) {
    return Value::FORWARD;
  } else if ((values & m_reverseMask) != 0) {
    return Value::REVERSE;
  } else {
    return Value::OFF;
  }
}

void DoubleSolenoid::Toggle() {
  Value value = Get();

  if (value == FORWARD) {
    Set(REVERSE);
  } else if (value == REVERSE) {
    Set(FORWARD);
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

void DoubleSolenoid::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Double Solenoid");
  builder.SetActuator(true);
  builder.AddSmallStringProperty(
      "Value",
      [=, this](wpi::SmallVectorImpl<char>& buf) -> std::string_view {
        switch (Get()) {
          case FORWARD:
            return "Forward";
          case REVERSE:
            return "Reverse";
          default:
            return "Off";
        }
      },
      [=, this](std::string_view value) {
        Value lvalue = OFF;
        if (value == "Forward") {
          lvalue = FORWARD;
        } else if (value == "Reverse") {
          lvalue = REVERSE;
        }
        Set(lvalue);
      });
}
