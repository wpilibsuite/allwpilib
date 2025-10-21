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

  /**
   * Sets the angle range for the setAngle call.
   * By default, this is 0 to 180 degrees.
   *
   * Maximum angle must be greater than minimum angle.
   *
   * @param minAngle Minimum angle
   * @param maxAngle Maximum angle
   */
  void SetAngleRange(units::degree_t minAngle, units::degree_t maxAngle);

  /**
   * Sets the PWM range for the servo.
   * By default, this is 600 to 2400 microseconds.
   *
   * Maximum must be greater than minimum.
   *
   * @param minPwm Minimum PWM
   * @param maxPwm Maximum PWM
   */
  void SetPWMRange(units::microsecond_t minPwm, units::microsecond_t maxPwm);

  /**
   * Sets whether the servo is reversed.
   *
   * This will reverse both Set() and SetAngle().
   *
   * @param reversed True to reverse, false for normal
   */
  void SetReversed(bool reversed);

  /**
   * Enables or disables continuous rotation mode.
   *
   * In continuous rotation mode, the servo will interpret
   * Set() commands to between -1.0 and 1.0, instead of 0.0 to 1.0.
   *
   * @param enable True to enable continuous rotation mode, false to disable
   */
  void SetContinousRotationMode(bool enable);

 private:
  units::microsecond_t GetFullRangeScaleFactor();
  units::degree_t GetServoAngleRange();

  ExpansionHub m_hub;
  int m_channel;

  units::degree_t m_maxServoAngle = 180.0_deg;
  units::degree_t m_minServoAngle = 0.0_deg;

  units::microsecond_t m_minPwm = 600_us;
  units::microsecond_t m_maxPwm = 2400_us;

  bool m_reversed = false;
  bool m_continousMode = false;

  nt::IntegerPublisher m_pulseWidthPublisher;
  nt::IntegerPublisher m_framePeriodPublisher;
  nt::BooleanPublisher m_enabledPublisher;
};
}  // namespace frc
