/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Relay.h"

#include <utility>

#include <hal/HAL.h>
#include <hal/Ports.h>
#include <hal/Relay.h>
#include <wpi/raw_ostream.h>

#include "frc/SensorUtil.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

Relay::Relay(int channel, Relay::Direction direction)
    : m_channel(channel), m_direction(direction) {
  if (!SensorUtil::CheckRelayChannel(m_channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "Relay Channel " + wpi::Twine(m_channel));
    return;
  }

  HAL_PortHandle portHandle = HAL_GetPort(channel);

  if (m_direction == kBothDirections || m_direction == kForwardOnly) {
    int32_t status = 0;
    m_forwardHandle = HAL_InitializeRelayPort(portHandle, true, &status);
    if (status != 0) {
      wpi_setErrorWithContextRange(status, 0, HAL_GetNumRelayChannels(),
                                   channel, HAL_GetErrorMessage(status));
      m_forwardHandle = HAL_kInvalidHandle;
      m_reverseHandle = HAL_kInvalidHandle;
      return;
    }
    HAL_Report(HALUsageReporting::kResourceType_Relay, m_channel);
  }
  if (m_direction == kBothDirections || m_direction == kReverseOnly) {
    int32_t status = 0;
    m_reverseHandle = HAL_InitializeRelayPort(portHandle, false, &status);
    if (status != 0) {
      wpi_setErrorWithContextRange(status, 0, HAL_GetNumRelayChannels(),
                                   channel, HAL_GetErrorMessage(status));
      m_forwardHandle = HAL_kInvalidHandle;
      m_reverseHandle = HAL_kInvalidHandle;
      return;
    }

    HAL_Report(HALUsageReporting::kResourceType_Relay, m_channel + 128);
  }

  int32_t status = 0;
  if (m_forwardHandle != HAL_kInvalidHandle) {
    HAL_SetRelay(m_forwardHandle, false, &status);
    if (status != 0) {
      wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
      m_forwardHandle = HAL_kInvalidHandle;
      m_reverseHandle = HAL_kInvalidHandle;
      return;
    }
  }
  if (m_reverseHandle != HAL_kInvalidHandle) {
    HAL_SetRelay(m_reverseHandle, false, &status);
    if (status != 0) {
      wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
      m_forwardHandle = HAL_kInvalidHandle;
      m_reverseHandle = HAL_kInvalidHandle;
      return;
    }
  }

  SetName("Relay", m_channel);
}

Relay::~Relay() {
  int32_t status = 0;
  HAL_SetRelay(m_forwardHandle, false, &status);
  HAL_SetRelay(m_reverseHandle, false, &status);
  // ignore errors, as we want to make sure a free happens.
  if (m_forwardHandle != HAL_kInvalidHandle) HAL_FreeRelayPort(m_forwardHandle);
  if (m_reverseHandle != HAL_kInvalidHandle) HAL_FreeRelayPort(m_reverseHandle);
}

Relay::Relay(Relay&& rhs)
    : MotorSafety(std::move(rhs)),
      SendableBase(std::move(rhs)),
      m_channel(std::move(rhs.m_channel)),
      m_direction(std::move(rhs.m_direction)) {
  std::swap(m_forwardHandle, rhs.m_forwardHandle);
  std::swap(m_reverseHandle, rhs.m_reverseHandle);
}

Relay& Relay::operator=(Relay&& rhs) {
  MotorSafety::operator=(std::move(rhs));
  SendableBase::operator=(std::move(rhs));

  m_channel = std::move(rhs.m_channel);
  m_direction = std::move(rhs.m_direction);
  std::swap(m_forwardHandle, rhs.m_forwardHandle);
  std::swap(m_reverseHandle, rhs.m_reverseHandle);

  return *this;
}

void Relay::Set(Relay::Value value) {
  if (StatusIsFatal()) return;

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
        wpi_setWPIError(IncompatibleMode);
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
        wpi_setWPIError(IncompatibleMode);
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

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

Relay::Value Relay::Get() const {
  int32_t status;

  if (m_direction == kForwardOnly) {
    if (HAL_GetRelay(m_forwardHandle, &status)) {
      return kOn;
    } else {
      return kOff;
    }
  } else if (m_direction == kReverseOnly) {
    if (HAL_GetRelay(m_reverseHandle, &status)) {
      return kOn;
    } else {
      return kOff;
    }
  } else {
    if (HAL_GetRelay(m_forwardHandle, &status)) {
      if (HAL_GetRelay(m_reverseHandle, &status)) {
        return kOn;
      } else {
        return kForward;
      }
    } else {
      if (HAL_GetRelay(m_reverseHandle, &status)) {
        return kReverse;
      } else {
        return kOff;
      }
    }
  }

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

int Relay::GetChannel() const { return m_channel; }

void Relay::StopMotor() { Set(kOff); }

void Relay::GetDescription(wpi::raw_ostream& desc) const {
  desc << "Relay " << GetChannel();
}

void Relay::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Relay");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { Set(kOff); });
  builder.AddSmallStringProperty(
      "Value",
      [=](wpi::SmallVectorImpl<char>& buf) -> wpi::StringRef {
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
      [=](wpi::StringRef value) {
        if (value == "Off")
          Set(kOff);
        else if (value == "Forward")
          Set(kForward);
        else if (value == "Reverse")
          Set(kReverse);
        else if (value == "On")
          Set(kOn);
      });
}
