// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>

#include "frc/PWM.h"

namespace frc {

/**
 * Common base class for all rotary servos.
 */
class RotaryServo : public PWM {
 public:
  RotaryServo(RotaryServo&&) = default;
  RotaryServo& operator=(RotaryServo&&) = default;

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
   * Set the servo to offline.
   *
   * Set the servo raw value to 0 (undriven)
   */
  void SetOffline();

  /**
   * Get the servo position.
   *
   * Servo values range from 0.0 to 1.0 corresponding to the range of full left
   * to full right. This returns the commanded position, not the position that
   * the servo is actually at, as the servo does not report its own position.
   *
   * @return Position from 0.0 to 1.0.
   */
  double Get() const;

  /**
   * Set the servo angle.
   *
   * Servo angles that are out of the supported range of the servo simply
   * "saturate" in that direction. In other words, if the servo has a range of
   * (X degrees to Y degrees) than angles of less than X result in an angle of
   * X being set and angles of more than Y degrees result in an angle of Y being
   * set.
   *
   * @param angle The angle in degrees to set the servo.
   */
  void SetAngle(units::radian_t angle);

  /**
   * Get the servo angle.
   *
   * This returns the commanded angle, not the angle that the servo is actually
   * at, as the servo does not report its own angle.
   *
   * @return The angle in radians to which the servo is set.
   */
  units::radian_t GetAngle() const;

  /**
   * Get the maximum angle of the servo.
   *
   * @return The maximum angle of the servo in radians.
   */
  units::radian_t GetMaxAngle() const;

  /**
   * Get the minimum angle of the servo.
   *
   * @return The minimum angle of the servo in radians.
   */
  units::radian_t GetMinAngle() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 protected:
  /**
   * Constructor for a rotary servo PWM.
   *
   * @param name Name to use for SendableRegistry.
   * @param channel The PWM channel to which the servo is attached. 0-9 are
   *                on-board, 10-19 are on the MXP port
   * @param minServoAngle Minimum servo angle in radians which can be commanded.
   * @param maxServoAngle Maximum servo angle in radians which can be commanded.
   */
  RotaryServo(std::string_view name, int channel, units::radian_t minServoAngle,
              units::radian_t maxServoAngle);

 private:
  units::radian_t GetServoAngleRange() const;

  units::radian_t m_minServoAngle;
  units::radian_t m_maxServoAngle;
};

}  // namespace frc
