// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DoubleSolenoid.h"

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

DoubleSolenoid::DoubleSolenoid(int forwardChannel, int reverseChannel)
    : DoubleSolenoid(SensorUtil::GetDefaultSolenoidModule(), forwardChannel,
                     reverseChannel) {}

DoubleSolenoid::DoubleSolenoid(int moduleNumber, int forwardChannel,
                               int reverseChannel)
    : SolenoidBase(moduleNumber),
      m_forwardChannel(forwardChannel),
      m_reverseChannel(reverseChannel) {
  if (!SensorUtil::CheckSolenoidModule(m_moduleNumber)) {
    throw FRC_MakeError(err::ModuleIndexOutOfRange, "Module {}",
                        m_moduleNumber);
  }
  if (!SensorUtil::CheckSolenoidChannel(m_forwardChannel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}",
                        m_forwardChannel);
  }
  if (!SensorUtil::CheckSolenoidChannel(m_reverseChannel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}",
                        m_reverseChannel);
  }
  int32_t status = 0;
  m_forwardHandle = HAL_InitializeSolenoidPort(
      HAL_GetPortWithModule(moduleNumber, m_forwardChannel), &status);
  FRC_CheckErrorStatus(status, "Module {} Channel {}", m_moduleNumber,
                       m_forwardChannel);

  m_reverseHandle = HAL_InitializeSolenoidPort(
      HAL_GetPortWithModule(moduleNumber, m_reverseChannel), &status);
  if (status != 0) {
    // free forward solenoid
    HAL_FreeSolenoidPort(m_forwardHandle);
    FRC_CheckErrorStatus(status, "Module {} Channel {}", m_moduleNumber,
                         m_reverseChannel);
    return;
  }

  m_forwardMask = 1 << m_forwardChannel;
  m_reverseMask = 1 << m_reverseChannel;

  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_forwardChannel + 1,
             m_moduleNumber + 1);
  HAL_Report(HALUsageReporting::kResourceType_Solenoid, m_reverseChannel + 1,
             m_moduleNumber + 1);

  SendableRegistry::GetInstance().AddLW(this, "DoubleSolenoid", m_moduleNumber,
                                        m_forwardChannel);
}

DoubleSolenoid::~DoubleSolenoid() {
  HAL_FreeSolenoidPort(m_forwardHandle);
  HAL_FreeSolenoidPort(m_reverseHandle);
}

void DoubleSolenoid::Set(Value value) {
  bool forward = false;
  bool reverse = false;

  switch (value) {
    case kOff:
      forward = false;
      reverse = false;
      break;
    case kForward:
      forward = true;
      reverse = false;
      break;
    case kReverse:
      forward = false;
      reverse = true;
      break;
  }

  int fstatus = 0;
  HAL_SetSolenoid(m_forwardHandle, forward, &fstatus);
  int rstatus = 0;
  HAL_SetSolenoid(m_reverseHandle, reverse, &rstatus);

  FRC_CheckErrorStatus(fstatus, "Module {} Channel {}", m_moduleNumber,
                       m_forwardChannel);
  FRC_CheckErrorStatus(rstatus, "Module {} Channel {}", m_moduleNumber,
                       m_reverseChannel);
}

DoubleSolenoid::Value DoubleSolenoid::Get() const {
  int fstatus = 0;
  int rstatus = 0;
  bool valueForward = HAL_GetSolenoid(m_forwardHandle, &fstatus);
  bool valueReverse = HAL_GetSolenoid(m_reverseHandle, &rstatus);

  FRC_CheckErrorStatus(fstatus, "Module {} Channel {}", m_moduleNumber,
                       m_forwardChannel);
  FRC_CheckErrorStatus(rstatus, "Module {} Channel {}", m_moduleNumber,
                       m_reverseChannel);

  if (valueForward) {
    return kForward;
  } else if (valueReverse) {
    return kReverse;
  } else {
    return kOff;
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

bool DoubleSolenoid::IsFwdSolenoidDisabledListed() const {
  int DisabledList = GetPCMSolenoidDisabledList(m_moduleNumber);
  return (DisabledList & m_forwardMask) != 0;
}

bool DoubleSolenoid::IsRevSolenoidDisabledListed() const {
  int DisabledList = GetPCMSolenoidDisabledList(m_moduleNumber);
  return (DisabledList & m_reverseMask) != 0;
}

void DoubleSolenoid::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Double Solenoid");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { Set(kOff); });
  builder.AddSmallStringProperty(
      "Value",
      [=](wpi::SmallVectorImpl<char>& buf) -> wpi::StringRef {
        switch (Get()) {
          case kForward:
            return "Forward";
          case kReverse:
            return "Reverse";
          default:
            return "Off";
        }
      },
      [=](wpi::StringRef value) {
        Value lvalue = kOff;
        if (value == "Forward") {
          lvalue = kForward;
        } else if (value == "Reverse") {
          lvalue = kReverse;
        }
        Set(lvalue);
      });
}
