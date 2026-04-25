// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/hardware/expansionhub/ExpansionHub.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/units/time.hpp"

namespace wpi {

/** This class controls a specific servo in continuous rotation mode hooked up
 * to an ExpansionHub. */
class ExpansionHubCRServo {
 public:
  /**
   * Constructs a continuous rotation servo at the requested channel on a
   * specific USB port.
   *
   * @sa ExpansionHubServo for a servo mode, or non-continuous rotation servo
   * @param usbId The USB port ID the hub is connected to
   * @param channel The servo channel
   */
  ExpansionHubCRServo(int usbId, int channel);
  ~ExpansionHubCRServo() noexcept;

  /**
   * Set the servo throttle.
   *
   * Throttle values range from -1.0 to 1.0 corresponding to full reverse to
   * full forward.
   *
   * @param value Throttle from -1.0 to 1.0.
   */
  void SetThrottle(double value);

  /**
   * Sets the raw pulse width output on the servo.
   *
   * @param pulseWidth Pulse width
   */
  void SetPulseWidth(wpi::units::microsecond_t pulseWidth);

  /**
   * Sets if the servo output is enabled or not. Defaults to false.
   *
   * @param enabled True to enable, false to disable
   */
  void SetEnabled(bool enabled);

  /**
   * Sets the frame period for the servo. Defaults to 20ms.
   *
   * @param framePeriod The frame period
   */
  void SetFramePeriod(wpi::units::microsecond_t framePeriod);

  /**
   * Gets if the underlying ExpansionHub is connected.
   *
   * @return True if hub is connected, otherwise false
   */
  bool IsHubConnected() const { return m_hub.IsHubConnected(); }

  /**
   * Sets the PWM range for the servo.
   * By default, this is 600 to 2400 microseconds.
   *
   * Maximum must be greater than minimum.
   *
   * @param minPwm Minimum PWM
   * @param maxPwm Maximum PWM
   */
  void SetPWMRange(wpi::units::microsecond_t minPwm,
                   wpi::units::microsecond_t maxPwm);

  /**
   * Sets whether the servo is reversed.
   *
   * This will reverse SetThrottle.
   *
   * @param reversed True to reverse, false for normal
   */
  void SetReversed(bool reversed);

 private:
  wpi::units::microsecond_t GetFullRangeScaleFactor() const;

  ExpansionHub m_hub;
  int m_channel;

  wpi::units::microsecond_t m_minPwm = 600_us;
  wpi::units::microsecond_t m_maxPwm = 2400_us;

  bool m_reversed = false;

  wpi::nt::IntegerPublisher m_pulseWidthPublisher;
  wpi::nt::IntegerPublisher m_framePeriodPublisher;
  wpi::nt::BooleanPublisher m_enabledPublisher;
};
}  // namespace wpi
