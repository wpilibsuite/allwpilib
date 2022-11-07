// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DoubleSolenoid.h"

#include <utility>

#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

DoubleSolenoid::DoubleSolenoid(int module, PneumaticsModuleType moduleType,
                               int forwardChannel, int reverseChannel)
    : m_module{PneumaticsBase::GetForType(module, moduleType)},
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

  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_forwardChannel + 1,
             m_module->GetModuleNumber() + 1);
  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_reverseChannel + 1,
             m_module->GetModuleNumber() + 1);

  wpi::SendableRegistry::AddLW(this, "DoubleSolenoid",
                               m_module->GetModuleNumber(), m_forwardChannel);
}

DoubleSolenoid::DoubleSolenoid(PneumaticsModuleType moduleType,
                               int forwardChannel, int reverseChannel)
    : DoubleSolenoid{PneumaticsBase::GetDefaultForType(moduleType), moduleType,
                     forwardChannel, reverseChannel} {}

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

void DoubleSolenoid::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Double Solenoid");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { Set(kOff); });
  builder.AddSmallStringProperty(
      "Value",
      [=, this](wpi::SmallVectorImpl<char>& buf) -> std::string_view {
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
