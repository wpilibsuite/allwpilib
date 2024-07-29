// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/MotorSafety.h"

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
class Relay : public MotorSafety,
              public wpi::Sendable,
              public wpi::SendableHelper<Relay> {
 public:
  /**
   * The state to drive a Relay to.
   */
  enum Value {
    /// Off.
    kOff,
    /// On.
    kOn,
    /// Forward.
    kForward,
    /// Reverse.
    kReverse
  };

  /**
   * The Direction(s) that a relay is configured to operate in.
   */
  enum Direction {
    /// Both directions are valid.
    kBothDirections,
    /// Only forward is valid.
    kForwardOnly,
    /// Only reverse is valid.
    kReverseOnly
  };

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

  Relay(Relay&&) = default;
  Relay& operator=(Relay&&) = default;

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

  std::string GetDescription() const override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  int m_channel;
  Direction m_direction;

  hal::Handle<HAL_RelayHandle> m_forwardHandle;
  hal::Handle<HAL_RelayHandle> m_reverseHandle;
};

}  // namespace frc
