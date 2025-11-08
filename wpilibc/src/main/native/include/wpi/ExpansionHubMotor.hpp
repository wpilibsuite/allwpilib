// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/current.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/voltage.hpp"

#include "wpi/ExpansionHub.hpp"
#include "wpi/ExpansionHubPidConstants.hpp"

namespace frc {

/** This class controls a specific motor and encoder hooked up to an
 * ExpansionHub. */
class ExpansionHubMotor {
 public:
  /**
   * Constructs a servo at the requested channel on a specific USB port.
   *
   * @param usbId The USB port ID the hub is connected to
   * @param channel The motor channel
   */
  ExpansionHubMotor(int usbId, int channel);
  ~ExpansionHubMotor() noexcept;

  /**
   * Sets the percentage power to run the motor at, between -1 and 1.
   *
   * @param power The power to drive the motor at
   */
  void SetPercentagePower(double power);

  /**
   * Sets the voltage to run the motor at. This value will be continously scaled
   * to match the input voltage.
   *
   * @param voltage The voltage to drive the motor at
   */
  void SetVoltage(units::volt_t voltage);

  /**
   * Command the motor to drive to a specific position setpoint. This value will
   * be scaled by SetDistancePerCount and influenced by the PID constants.
   *
   * @param setpoint The position setpoint to drive the motor to
   */
  void SetPositionSetpoint(double setpoint);

  /**
   * Command the motor to drive to a specific velocity setpoint. This value will
   * be scaled by SetDistancePerCount and influenced by the PID constants.
   *
   * @param setpoint The velocity setpoint to drive the motor to
   */
  void SetVelocitySetpoint(double setpoint);

  /**
   * Sets if the motor output is enabled or not. Defaults to false.
   *
   * @param enabled True to enable, false to disable
   */
  void SetEnabled(bool enabled);

  /**
   * Sets if the motor should float or brake when 0 is commanded. Defaults to
   * false.
   *
   * @param floatOn0 True to float when commanded 0, false to brake
   */
  void SetFloatOn0(bool floatOn0);

  /**
   * Gets the current being pulled by the motor.
   *
   * @return Motor current
   */
  units::ampere_t GetCurrent() const;

  /**
   * Sets the distance per count of the encoder. Used to scale encoder readings.
   *
   * @param perCount The distance moved per count of the encoder
   */
  void SetDistancePerCount(double perCount);

  /**
   * Gets the current velocity of the motor encoder. Scaled into
   * distancePerCount units.
   *
   * @return Encoder velocity
   */
  double GetEncoderVelocity() const;

  /**
   * Gets the current position of the motor encoder. Scaled into
   * distancePerCount units.
   *
   * @return Encoder position
   */
  double GetEncoderPosition() const;

  /**
   * Sets if the motor and encoder should be reversed.
   *
   * @param reversed True to reverse encoder, false otherwise
   */
  void SetReversed(bool reversed);

  /** Reset the encoder count to 0. */
  void ResetEncoder();

  /**
   * Gets the PID constants object for velocity PID.
   *
   * @return Velocity PID constants object
   */
  ExpansionHubPidConstants& GetVelocityPidConstants();

  /**
   * Gets the PID constants object for position PID.
   *
   * @return Position PID constants object
   */
  ExpansionHubPidConstants& GetPositionPidConstants();

  /**
   * Gets if the underlying ExpansionHub is connected.
   *
   * @return True if hub is connected, otherwise false
   */
  bool IsHubConnected() { return m_hub.IsHubConnected(); }

  /**
   * Sets this motor to follow another motor on the same hub.
   *
   * This does not support following motors that are also followers.
   * Additionally, the direction of both motors will be the same.
   *
   * @param leader The motor to follow
   */
  void Follow(const ExpansionHubMotor& leader);

 private:
  ExpansionHub m_hub;
  int m_channel;

  nt::DoubleSubscriber m_encoderSubscriber;
  nt::DoubleSubscriber m_encoderVelocitySubscriber;
  nt::DoubleSubscriber m_currentSubscriber;

  nt::DoublePublisher m_setpointPublisher;
  nt::BooleanPublisher m_floatOn0Publisher;
  nt::BooleanPublisher m_enabledPublisher;

  nt::IntegerPublisher m_modePublisher;

  nt::BooleanPublisher m_reversedPublisher;
  nt::BooleanPublisher m_resetEncoderPublisher;

  nt::DoublePublisher m_distancePerCountPublisher;

  ExpansionHubPidConstants m_velocityPidConstants;
  ExpansionHubPidConstants m_positionPidConstants;
};
}  // namespace frc
