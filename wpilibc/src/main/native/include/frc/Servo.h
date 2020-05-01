/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/PWM.h"
#include "frc/SpeedController.h"

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
   * to full right.
   *
   * @return Position from 0.0 to 1.0.
   */
  double Get() const;

  /**
   * Set the servo angle.
   *
   * Assume that the servo angle is linear with respect to the PWM value (big
   * assumption, need to test).
   *
   * Servo angles that are out of the supported range of the servo simply
   * "saturate" in that direction. In other words, if the servo has a range of
   * (X degrees to Y degrees) than angles of less than X result in an angle of
   * X being set and angles of more than Y degrees result in an angle of Y being
   * set.
   *
   * @param degrees The angle in degrees to set the servo.
   */
  void SetAngle(double angle);

  /**
   * Get the servo angle.
   *
   * Assume that the servo angle is linear with respect to the PWM value (big
   * assumption, need to test).
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

  void InitSendable(SendableBuilder& builder) override;

 private:
  double GetServoAngleRange() const;

  static constexpr double kMaxServoAngle = 180.0;
  static constexpr double kMinServoAngle = 0.0;

  static constexpr double kDefaultMaxServoPWM = 2.4;
  static constexpr double kDefaultMinServoPWM = 0.6;
};

}  // namespace frc
