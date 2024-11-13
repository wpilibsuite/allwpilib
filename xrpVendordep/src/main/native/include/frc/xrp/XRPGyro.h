// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/geometry/Rotation2d.h>

#include <hal/SimDevice.h>
#include <units/angle.h>
#include <units/angular_velocity.h>

namespace frc {

/**
 * @ingroup xrp_api
 * @{
 */

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 *
 * This class is for the XRP onboard gyro, and will only work in
 * simulation/XRP mode. Only one instance of a XRPGyro is supported.
 */
class XRPGyro {
 public:
  /**
   * Constructs an XRPGyro.
   *
   * <p>Only one instance of a XRPGyro is supported.
   */
  XRPGyro();

  /**
   * Return the actual angle in radians that the robot is currently facing.
   *
   * The angle is based on integration of the returned rate form the gyro.
   * The angle is continuous, that is, it will continue from 2π->2.1π.
   * This allows algorithms that wouldn't want to see a discontinuity in the
   * gyro output as it sweeps from 2π to 0 radians on the second time around.
   *
   * @return the current heading of the robot in radians.
   */
  units::radian_t GetAngle() const;

  /**
   * Gets the angle the robot is facing.
   *
   * @return A Rotation2d with the current heading.
   */
  frc::Rotation2d GetRotation2d() const;

  /**
   * Return the rate of rotation of the gyro
   *
   * The rate is based on the most recent reading of the gyro.
   *
   * @return the current rate in radians per second
   */
  units::radians_per_second_t GetRate() const;

  /**
   * Gets the rate of turn in radians-per-second around the X-axis.
   *
   * @return rate of turn in radians-per-second
   */
  units::radians_per_second_t GetRateX() const;

  /**
   * Gets the rate of turn in radians-per-second around the Y-axis.
   *
   * @return rate of turn in radians-per-second
   */
  units::radians_per_second_t GetRateY() const;

  /**
   * Gets the rate of turn in radians-per-second around the Z-axis.
   *
   * @return rate of turn in radians-per-second
   */
  units::radians_per_second_t GetRateZ() const;

  /**
   * Gets the currently reported angle around the X-axis.
   *
   * @return current angle around X-axis in radians
   */
  units::radian_t GetAngleX() const;

  /**
   * Gets the currently reported angle around the Y-axis.
   *
   * @return current angle around Y-axis in radians
   */
  units::radian_t GetAngleY() const;

  /**
   * Gets the currently reported angle around the Z-axis.
   *
   * @return current angle around Z-axis in radians
   */
  units::radian_t GetAngleZ() const;

  /**
   * Reset the gyro angles to 0.
   */
  void Reset();

 private:
  hal::SimDevice m_simDevice;
  hal::SimDouble m_simRateX;
  hal::SimDouble m_simRateY;
  hal::SimDouble m_simRateZ;
  hal::SimDouble m_simAngleX;
  hal::SimDouble m_simAngleY;
  hal::SimDouble m_simAngleZ;

  units::radian_t m_angleXOffset = 0_rad;
  units::radian_t m_angleYOffset = 0_rad;
  units::radian_t m_angleZOffset = 0_rad;
};

/** @} */

}  // namespace frc
