/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/raw_ostream.h>

#include "frc/ErrorBase.h"
#include "frc/MotorSafety.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

/**
 * Class for Spike style relay outputs.
 *
 * Relays are intended to be connected to spikes or similar relays. The relay
 * channels controls a pair of pins that are either both off, one on, the other
 * on, or both on. This translates into two spike outputs at 0v, one at 12v and
 * one at 0v, one at 0v and the other at 12v, or two spike outputs at 12V. This
 * allows off, full forward, or full reverse control of motors without variable
 * speed. It also allows the two channels (forward and reverse) to be used
 * independently for something that does not care about voltage polarity (like
 * a solenoid).
 */
class Relay : public MotorSafety, public SendableBase {
 public:
  enum Value { kOff, kOn, kForward, kReverse };
  enum Direction { kBothDirections, kForwardOnly, kReverseOnly };

  /**
   * Relay constructor given a channel.
   *
   * This code initializes the relay and reserves all resources that need to be
   * locked. Initially the relay is set to both lines at 0v.
   *
   * @param channel   The channel number (0-3).
   * @param direction The direction that the Relay object will control.
   */
  explicit Relay(int channel, Direction direction = kBothDirections);

  /**
   * Free the resource associated with a relay.
   *
   * The relay channels are set to free and the relay output is turned off.
   */
  ~Relay() override;

  Relay(Relay&& rhs);
  Relay& operator=(Relay&& rhs);

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
  void Set(Value value);

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
  Value Get() const;

  int GetChannel() const;

  // MotorSafety interface
  void StopMotor() override;

  void GetDescription(wpi::raw_ostream& desc) const override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  int m_channel;
  Direction m_direction;

  HAL_RelayHandle m_forwardHandle = HAL_kInvalidHandle;
  HAL_RelayHandle m_reverseHandle = HAL_kInvalidHandle;
};

}  // namespace frc
