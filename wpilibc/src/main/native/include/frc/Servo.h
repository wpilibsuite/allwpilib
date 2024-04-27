// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>

#include "frc/PWM.h"

namespace frc {

/**
 * Standard hobby style servo.
 *
 * The range parameters default to the appropriate values for the Hitec HS-322HD
 * servo provided in the FIRST Kit of Parts in 2008.
 */
class Servo : public PWM {
 public:
  /**
   * Constructor.
   *
   * By default, 2.4 ms is used as the max PWM value and 0.6 ms is used as the
   * min PWM value.
   *
   * @param channel The PWM channel to which the servo is attached. 0-9 are
   *                on-board, 10-19 are on the MXP port
   */
  explicit Servo(int channel);

  Servo(Servo&&) = default;
  Servo& operator=(Servo&&) = default;

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
   * The angles are based on the HS-322HD Servo, and have a range of 0 to 180
   * degrees.
   *
   * Servo angles that are out of the supported range of the servo simply
   * "saturate" in that direction. In other words, if the servo has a range of
   * (X degrees to Y degrees) than angles of less than X result in an angle of
   * X being set and angles of more than Y degrees result in an angle of Y being
   * set.
   *
   * @param angle The angle in degrees to set the servo.
   */
  void SetAngle(double angle);

  /**
   * Get the servo angle.
   *
   * This returns the commanded angle, not the angle that the servo is actually
   * at, as the servo does not report its own angle.
   *
   * @return The angle in degrees to which the servo is set.
   */
  double GetAngle() const;

  /**
   * Get the maximum angle of the servo.
   *
   * @return The maximum angle of the servo in degrees.
   */
  double GetMaxAngle() const;

  /**
   * Get the minimum angle of the servo.
   *
   * @return The minimum angle of the servo in degrees.
   */
  double GetMinAngle() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  double GetServoAngleRange() const;

  static constexpr double kMaxServoAngle = 180.;
  static constexpr double kMinServoAngle = 0.0;

  static constexpr units::millisecond_t kDefaultMaxServoPWM = 2.4_ms;
  static constexpr units::millisecond_t kDefaultMinServoPWM = 0.6_ms;
};

}  // namespace frc
