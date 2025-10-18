// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include "frc/ExpansionHub.h"

#include <units/angle.h>
#include <units/time.h>

#include <networktables/BooleanTopic.h>
#include <networktables/IntegerTopic.h>

namespace frc {

/** This class controls a specific servo hooked up to an ExpansionHub. */
class ExpansionHubServo {
 public:
  /**
   * Constructs a servo at the requested channel on a specific USB port.
   *
   * @param usbId The USB port ID the hub is connected to
   * @param channel The servo channel
   */
  ExpansionHubServo(int usbId, int channel);
  ~ExpansionHubServo() noexcept;

  /**
   * Set the servo position.
   *
   * Servo values range from 0.0 to 1.0 corresponding to the range of full left
   * to full right.
   *
   * @param value Position from 0.0 to 1.0.
   */
  void Set(double value);

  /**
   * Sets the servo angle
   *
   * Servo angles range from 0 to 180 degrees. Use Set() with your own scaler
   * for other angle ranges.
   *
   * @param angle Position in angle units. Will be scaled between 0 and 180
   * degrees
   */
  void SetAngle(units::degree_t angle);

  /**
   * Sets the raw pulse width output on the servo.
   *
   * @param pulseWidth Pulse width
   */
  void SetPulseWidth(units::microsecond_t pulseWidth);

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
  void SetFramePeriod(units::microsecond_t framePeriod);

  /**
   * Gets if the underlying ExpansionHub is connected.
   *
   * @return True if hub is connected, otherwise false
   */
  bool IsHubConnected() const { return m_hub.IsHubConnected(); }

 private:
  units::microsecond_t GetFullRangeScaleFactor();
  units::degree_t GetServoAngleRange();

  ExpansionHub m_hub;
  int m_channel;

  static constexpr units::degree_t kMaxServoAngle = 180.0_deg;
  static constexpr units::degree_t kMinServoAngle = 0.0_deg;

  static constexpr units::microsecond_t kDefaultMaxServoPWM = 2400_us;
  static constexpr units::microsecond_t kDefaultMinServoPWM = 600_us;

  static constexpr units::microsecond_t m_minPwm = kDefaultMinServoPWM;
  static constexpr units::microsecond_t m_maxPwm = kDefaultMaxServoPWM;

  nt::IntegerPublisher m_pulseWidthPublisher;
  nt::IntegerPublisher m_framePeriodPublisher;
  nt::BooleanPublisher m_enabledPublisher;
};
}  // namespace frc
