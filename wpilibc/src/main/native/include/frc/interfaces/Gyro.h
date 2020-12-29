// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>

#include "frc/geometry/Rotation2d.h"

namespace frc {

/**
 * Interface for yaw rate gyros.
 */
class Gyro {
 public:
  Gyro() = default;
  virtual ~Gyro() = default;

  Gyro(Gyro&&) = default;
  Gyro& operator=(Gyro&&) = default;

  /**
   * Calibrate the gyro. It's important to make sure that the robot is not
   * moving while the calibration is in progress, this is typically
   * done when the robot is first turned on while it's sitting at rest before
   * the match starts.
   */
  virtual void Calibrate() = 0;

  /**
   * Reset the gyro. Resets the gyro to a heading of zero. This can be used if
   * there is significant drift in the gyro and it needs to be recalibrated
   * after it has been running.
   */
  virtual void Reset() = 0;

  /**
   * Return the heading of the robot in degrees.
   *
   * The angle is continuous, that is it will continue from 360 to 361 degrees.
   * This allows algorithms that wouldn't want to see a discontinuity in the
   * gyro output as it sweeps past from 360 to 0 on the second time around.
   *
   * The angle is expected to increase as the gyro turns clockwise when looked
   * at from the top. It needs to follow the NED axis convention.
   *
   * @return the current heading of the robot in degrees. This heading is based
   *         on integration of the returned rate from the gyro.
   */
  virtual double GetAngle() const = 0;

  /**
   * Return the rate of rotation of the gyro.
   *
   * The rate is based on the most recent reading of the gyro analog value.
   *
   * The rate is expected to be positive as the gyro turns clockwise when looked
   * at from the top. It needs to follow the NED axis convention.
   *
   * @return the current rate in degrees per second
   */
  virtual double GetRate() const = 0;

  /**
   * Return the heading of the robot as a Rotation2d.
   *
   * The angle is continuous, that is it will continue from 360 to 361 degrees.
   * This allows algorithms that wouldn't want to see a discontinuity in the
   * gyro output as it sweeps past from 360 to 0 on the second time around.
   *
   * The angle is expected to increase as the gyro turns counterclockwise when
   * looked at from the top. It needs to follow the NWU axis convention.
   *
   * @return the current heading of the robot as a Rotation2d. This heading is
   *         based on integration of the returned rate from the gyro.
   */
  virtual Rotation2d GetRotation2d() const {
    return Rotation2d{units::degree_t{-GetAngle()}};
  }
};

}  // namespace frc
