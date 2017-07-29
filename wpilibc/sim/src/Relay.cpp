/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Relay.h"

#include "LiveWindow/LiveWindow.h"
#include "MotorSafetyHelper.h"
#include "WPIErrors.h"
#include "llvm/SmallString.h"

/**
 * Relay constructor given a channel.
 *
 * This code initializes the relay and reserves all resources that need to be
 * locked. Initially the relay is set to both lines at 0v.
 *
 * @param channel   The channel number (0-3).
 * @param direction The direction that the Relay object will control.
 */
frc::Relay::Relay(int channel, frc::Relay::Direction direction)
    : m_channel(channel), m_direction(direction) {
  llvm::SmallString<32> buf;
  llvm::raw_svector_ostream oss(buf);
  if (!SensorBase::CheckRelayChannel(m_channel)) {
    oss << "Relay Channel " << m_channel;
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, oss.str());
    return;
  }

  m_safetyHelper = std::make_unique<MotorSafetyHelper>(this);
  m_safetyHelper->SetSafetyEnabled(false);

  oss << "relay/" << m_channel;

  // TODO: Allow two different relays (targetting the different halves of a
  // relay) to be combined to control one motor.
  impl = new SimContinuousOutput(oss.str());
  LiveWindow::GetInstance()->AddActuator("Relay", 1, m_channel, this);
  go_pos = go_neg = false;
}

/**
 * Free the resource associated with a relay.
 *
 * The relay channels are set to free and the relay output is turned off.
 */
frc::Relay::~Relay() {
  impl->Set(0);
  if (m_table != nullptr) m_table->RemoveTableListener(this);
}

/**
 * Set the relay state.
 *
 * Valid values depend on which directions of the relay are controlled by the
 * object.
 *
 * When set to kBothDirections, the relay can be any of the four states:
 *    0v-0v, 0v-12v, 12v-0v, 12v-12v
 *
 * When set to kForwardOnly or kReverseOnly, you can specify the constant for
 * the direction or you can simply specify kOff and kOn.  Using only kOff and
 * kOn is recommended.
 *
 * @param value The state to set the relay.
 */
void frc::Relay::Set(frc::Relay::Value value) {
  switch (value) {
    case kOff:
      if (m_direction == kBothDirections || m_direction == kForwardOnly) {
        go_pos = false;
      }
      if (m_direction == kBothDirections || m_direction == kReverseOnly) {
        go_neg = false;
      }
      break;
    case kOn:
      if (m_direction == kBothDirections || m_direction == kForwardOnly) {
        go_pos = true;
      }
      if (m_direction == kBothDirections || m_direction == kReverseOnly) {
        go_neg = true;
      }
      break;
    case kForward:
      if (m_direction == kReverseOnly) {
        wpi_setWPIError(IncompatibleMode);
        break;
      }
      if (m_direction == kBothDirections || m_direction == kForwardOnly) {
        go_pos = true;
      }
      if (m_direction == kBothDirections) {
        go_neg = false;
      }
      break;
    case kReverse:
      if (m_direction == kForwardOnly) {
        wpi_setWPIError(IncompatibleMode);
        break;
      }
      if (m_direction == kBothDirections) {
        go_pos = false;
      }
      if (m_direction == kBothDirections || m_direction == kReverseOnly) {
        go_neg = true;
      }
      break;
  }
  impl->Set((go_pos ? 1 : 0) + (go_neg ? -1 : 0));
}

/**
 * Get the Relay State
 *
 * Gets the current state of the relay.
 *
 * When set to kForwardOnly or kReverseOnly, value is returned as kOn/kOff not
 * kForward/kReverse (per the recommendation in Set).
 *
 * @return The current state of the relay as a frc::Relay::Value
 */
frc::Relay::Value frc::Relay::Get() const {
  // TODO: Don't assume that the go_pos and go_neg fields are correct?
  if ((go_pos || m_direction == kReverseOnly) &&
      (go_neg || m_direction == kForwardOnly)) {
    return kOn;
  } else if (go_pos) {
    return kForward;
  } else if (go_neg) {
    return kReverse;
  } else {
    return kOff;
  }
}

int frc::Relay::GetChannel() const { return m_channel; }

/**
 * Set the expiration time for the Relay object.
 *
 * @param timeout The timeout (in seconds) for this relay object
 */
void frc::Relay::SetExpiration(double timeout) {
  m_safetyHelper->SetExpiration(timeout);
}

/**
 * Return the expiration time for the relay object.
 *
 * @return The expiration time value.
 */
double frc::Relay::GetExpiration() const {
  return m_safetyHelper->GetExpiration();
}

/**
 * Check if the relay object is currently alive or stopped due to a timeout.
 *
 * @return a bool value that is true if the motor has NOT timed out and should
 *         still be running.
 */
bool frc::Relay::IsAlive() const { return m_safetyHelper->IsAlive(); }

/**
 * Stop the motor associated with this PWM object.
 *
 * This is called by the MotorSafetyHelper object when it has a timeout for this
 * relay and needs to stop it from running.
 */
void frc::Relay::StopMotor() { Set(kOff); }

/**
 * Enable/disable motor safety for this device
 *
 * Turn on and off the motor safety option for this relay object.
 *
 * @param enabled True if motor safety is enforced for this object
 */
void frc::Relay::SetSafetyEnabled(bool enabled) {
  m_safetyHelper->SetSafetyEnabled(enabled);
}

/**
 * Check if motor safety is enabled for this object.
 *
 * @return True if motor safety is enforced for this object
 */
bool frc::Relay::IsSafetyEnabled() const {
  return m_safetyHelper->IsSafetyEnabled();
}

void frc::Relay::GetDescription(llvm::raw_ostream& desc) const {
  desc << "Relay " << GetChannel();
}

void frc::Relay::ValueChanged(ITable* source, llvm::StringRef key,
                              std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsString()) return;
  if (value->GetString() == "Off")
    Set(kOff);
  else if (value->GetString() == "Forward")
    Set(kForward);
  else if (value->GetString() == "Reverse")
    Set(kReverse);
}

void frc::Relay::UpdateTable() {
  if (m_table != nullptr) {
    if (Get() == kOn) {
      m_table->PutString("Value", "On");
    } else if (Get() == kForward) {
      m_table->PutString("Value", "Forward");
    } else if (Get() == kReverse) {
      m_table->PutString("Value", "Reverse");
    } else {
      m_table->PutString("Value", "Off");
    }
  }
}

void frc::Relay::StartLiveWindowMode() {
  if (m_table != nullptr) {
    m_table->AddTableListener("Value", this, true);
  }
}

void frc::Relay::StopLiveWindowMode() {
  if (m_table != nullptr) {
    m_table->RemoveTableListener(this);
  }
}

std::string frc::Relay::GetSmartDashboardType() const { return "Relay"; }

void frc::Relay::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> frc::Relay::GetTable() const { return m_table; }
