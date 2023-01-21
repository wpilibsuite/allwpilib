// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/length.h>

#include "frc/PWM.h"

namespace frc {

/**
 * Common base class for all linear servos.
 */
class LinearServo : public PWM {
 public:
  LinearServo(LinearServo&&) = default;
  LinearServo& operator=(LinearServo&&) = default;

  /**
   * Set the servo position.
   *
   * Servo values range from 0.0 to 1.0 corresponding to the range of full
   * retract to full extend.
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
   * Servo values range from 0.0 to 1.0 corresponding to the range of full
   * retract to full extend. This returns the commanded position, not the
   * position that the servo is actually at, as the servo does not report its
   * own position.
   *
   * @return Position from 0.0 to 1.0.
   */
  double Get() const;

  /**
   * Set the servo extend distance.
   *
   * Distances that are out of the supported range of the servo simply
   * "saturate" in that direction. In other words, if the servo has a stroke of
   * 50mm then distances of less than 0mm result in the servo being fully
   * retracted and distances of greater than 50mm result in the servo being
   * fully extended.
   *
   * @param distance The extend distance in metres to set the servo.
   */
  void SetExtendDistance(units::meter_t distance);

  /**
   * Get the servo extend distance
   *
   * This returns the commanded distance, not the distance that the servo is
   * actually at, as the servo does not report its own distance.
   *
   * @return The distance in metres to which the servo is set.
   */
  units::meter_t GetExtendDistance() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 protected:
  /**
   * Constructor for a linear servo PWM.
   *
   * @param name Name to use for SendableRegistry.
   * @param channel The PWM channel to which the servo is attached. 0-9 are
   *                on-board, 10-19 are on the MXP port.
   * @param strokeM Stroke distance of the linear servo.
   */
  LinearServo(std::string_view name, int channel, units::meter_t stroke);

 private:
  units::meter_t m_stroke;
};

}  // namespace frc
