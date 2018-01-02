/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Relay.h"

#include <HAL/HAL.h>
#include <HAL/Ports.h>
#include <HAL/Relay.h>
#include <llvm/raw_ostream.h>

#include "MotorSafetyHelper.h"
#include "SensorBase.h"
#include "SmartDashboard/SendableBuilder.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Relay constructor given a channel.
 *
 * This code initializes the relay and reserves all resources that need to be
 * locked. Initially the relay is set to both lines at 0v.
 *
 * @param channel   The channel number (0-3).
 * @param direction The direction that the Relay object will control.
 */
Relay::Relay(int channel, Relay::Direction direction)
    : m_channel(channel), m_direction(direction) {
  if (!SensorBase::CheckRelayChannel(m_channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "Relay Channel " + llvm::Twine(m_channel));
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

  m_safetyHelper = std::make_unique<MotorSafetyHelper>(this);
  m_safetyHelper->SetSafetyEnabled(false);

  SetName("Relay", m_channel);
}

/**
 * Free the resource associated with a relay.
 *
 * The relay channels are set to free and the relay output is turned off.
 */
Relay::~Relay() {
  int32_t status = 0;
  HAL_SetRelay(m_forwardHandle, false, &status);
  HAL_SetRelay(m_reverseHandle, false, &status);
  // ignore errors, as we want to make sure a free happens.
  if (m_forwardHandle != HAL_kInvalidHandle) HAL_FreeRelayPort(m_forwardHandle);
  if (m_reverseHandle != HAL_kInvalidHandle) HAL_FreeRelayPort(m_reverseHandle);
}

/**
 * Set the relay state.
 *
 * Valid values depend on which directions of the relay are controlled by the
 * object.
 *
 * When set to kBothDirections, the relay can be any of the four states:
 * 0v-0v, 0v-12v, 12v-0v, 12v-12v
 *
 * When set to kForwardOnly or kReverseOnly, you can specify the constant for
 * the direction or you can simply specify kOff and kOn.  Using only kOff and
 * kOn is recommended.
 *
 * @param value The state to set the relay.
 */
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

/**
 * Get the Relay State
 *
 * Gets the current state of the relay.
 *
 * When set to kForwardOnly or kReverseOnly, value is returned as kOn/kOff not
 * kForward/kReverse (per the recommendation in Set).
 *
 * @return The current state of the relay as a Relay::Value
 */
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

/**
 * Set the expiration time for the Relay object.
 *
 * @param timeout The timeout (in seconds) for this relay object
 */
void Relay::SetExpiration(double timeout) {
  m_safetyHelper->SetExpiration(timeout);
}

/**
 * Return the expiration time for the relay object.
 *
 * @return The expiration time value.
 */
double Relay::GetExpiration() const { return m_safetyHelper->GetExpiration(); }

/**
 * Check if the relay object is currently alive or stopped due to a timeout.
 *
 * @return a bool value that is true if the motor has NOT timed out and should
 *         still be running.
 */
bool Relay::IsAlive() const { return m_safetyHelper->IsAlive(); }

/**
 * Stop the motor associated with this PWM object.
 *
 * This is called by the MotorSafetyHelper object when it has a timeout for this
 * relay and needs to stop it from running.
 */
void Relay::StopMotor() { Set(kOff); }

/**
 * Enable/disable motor safety for this device.
 *
 * Turn on and off the motor safety option for this relay object.
 *
 * @param enabled True if motor safety is enforced for this object
 */
void Relay::SetSafetyEnabled(bool enabled) {
  m_safetyHelper->SetSafetyEnabled(enabled);
}

/**
 * Check if motor safety is enabled for this object.
 *
 * @returns True if motor safety is enforced for this object
 */
bool Relay::IsSafetyEnabled() const {
  return m_safetyHelper->IsSafetyEnabled();
}

void Relay::GetDescription(llvm::raw_ostream& desc) const {
  desc << "Relay " << GetChannel();
}

void Relay::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Relay");
  builder.SetSafeState([=]() { Set(kOff); });
  builder.AddSmallStringProperty(
      "Value",
      [=](llvm::SmallVectorImpl<char>& buf) -> llvm::StringRef {
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
      [=](llvm::StringRef value) {
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
