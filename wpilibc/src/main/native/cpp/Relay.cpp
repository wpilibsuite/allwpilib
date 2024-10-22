// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Relay.h"

#include <string>

#include <fmt/format.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>
#include <hal/Relay.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

Relay::Relay(int channel, Relay::Direction direction)
    : m_channel(channel), m_direction(direction) {
  if (!SensorUtil::CheckRelayChannel(m_channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", m_channel);
    return;
  }

  HAL_PortHandle portHandle = HAL_GetPort(channel);

  if (m_direction == kBothDirections || m_direction == kForwardOnly) {
    int32_t status = 0;
    std::string stackTrace = wpi::GetStackTrace(1);
    m_forwardHandle =
        HAL_InitializeRelayPort(portHandle, true, stackTrace.c_str(), &status);
    FRC_CheckErrorStatus(status, "Channel {}", m_channel);
    HAL_Report(HALUsageReporting::kResourceType_Relay, m_channel + 1);
  }
  if (m_direction == kBothDirections || m_direction == kReverseOnly) {
    int32_t status = 0;
    std::string stackTrace = wpi::GetStackTrace(1);
    m_reverseHandle =
        HAL_InitializeRelayPort(portHandle, false, stackTrace.c_str(), &status);
    FRC_CheckErrorStatus(status, "Channel {}", m_channel);
    HAL_Report(HALUsageReporting::kResourceType_Relay, m_channel + 128);
  }

  int32_t status = 0;
  if (m_forwardHandle != HAL_kInvalidHandle) {
    HAL_SetRelay(m_forwardHandle, false, &status);
    FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  }
  if (m_reverseHandle != HAL_kInvalidHandle) {
    HAL_SetRelay(m_reverseHandle, false, &status);
    FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  }

  wpi::SendableRegistry::AddLW(this, "Relay", m_channel);
}

Relay::~Relay() {
  int32_t status = 0;
  if (m_forwardHandle != HAL_kInvalidHandle) {
    HAL_SetRelay(m_forwardHandle, false, &status);
  }
  if (m_reverseHandle != HAL_kInvalidHandle) {
    HAL_SetRelay(m_reverseHandle, false, &status);
  }
}

void Relay::Set(Relay::Value value) {
  int32_t status = 0;

  switch (value) {
    case kOff:
      if (m_direction == kBothDirections || m_direction == kForwardOnly) {
        HAL_SetRelay(m_forwardHandle, false, &status);
      }
      if (m_direction == kBothDirections || m_direction == kReverseOnly) {
        HAL_SetRelay(m_reverseHandle, false, &status);
      }
      break;
    case kOn:
      if (m_direction == kBothDirections || m_direction == kForwardOnly) {
        HAL_SetRelay(m_forwardHandle, true, &status);
      }
      if (m_direction == kBothDirections || m_direction == kReverseOnly) {
        HAL_SetRelay(m_reverseHandle, true, &status);
      }
      break;
    case kForward:
      if (m_direction == kReverseOnly) {
        FRC_ReportError(err::IncompatibleMode, "channel {} setting {}",
                        m_channel, "forward");
        break;
      }
      if (m_direction == kBothDirections || m_direction == kForwardOnly) {
        HAL_SetRelay(m_forwardHandle, true, &status);
      }
      if (m_direction == kBothDirections) {
        HAL_SetRelay(m_reverseHandle, false, &status);
      }
      break;
    case kReverse:
      if (m_direction == kForwardOnly) {
        FRC_ReportError(err::IncompatibleMode, "channel {} setting {}",
                        m_channel, "reverse");
        break;
      }
      if (m_direction == kBothDirections) {
        HAL_SetRelay(m_forwardHandle, false, &status);
      }
      if (m_direction == kBothDirections || m_direction == kReverseOnly) {
        HAL_SetRelay(m_reverseHandle, true, &status);
      }
      break;
  }

  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

Relay::Value Relay::Get() const {
  Relay::Value value = kOff;
  int32_t status = 0;

  if (m_direction == kForwardOnly) {
    if (HAL_GetRelay(m_forwardHandle, &status)) {
      value = kOn;
    } else {
      value = kOff;
    }
  } else if (m_direction == kReverseOnly) {
    if (HAL_GetRelay(m_reverseHandle, &status)) {
      value = kOn;
    } else {
      value = kOff;
    }
  } else {
    if (HAL_GetRelay(m_forwardHandle, &status)) {
      if (HAL_GetRelay(m_reverseHandle, &status)) {
        value = kOn;
      } else {
        value = kForward;
      }
    } else {
      if (HAL_GetRelay(m_reverseHandle, &status)) {
        value = kReverse;
      } else {
        value = kOff;
      }
    }
  }

  FRC_CheckErrorStatus(status, "Channel {}", m_channel);

  return value;
}

int Relay::GetChannel() const {
  return m_channel;
}

void Relay::StopMotor() {
  Set(kOff);
}

std::string Relay::GetDescription() const {
  return fmt::format("Relay {}", GetChannel());
}

void Relay::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Relay");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { Set(kOff); });
  builder.AddSmallStringProperty(
      "Value",
      [=, this](wpi::SmallVectorImpl<char>& buf) -> std::string_view {
        switch (Get()) {
          case kOn:
            return "On";
          case kForward:
            return "Forward";
          case kReverse:
            return "Reverse";
          default:
            return "Off";
        }
      },
      [=, this](std::string_view value) {
        if (value == "Off") {
          Set(kOff);
        } else if (value == "Forward") {
          Set(kForward);
        } else if (value == "Reverse") {
          Set(kReverse);
        } else if (value == "On") {
          Set(kOn);
        }
      });
}
