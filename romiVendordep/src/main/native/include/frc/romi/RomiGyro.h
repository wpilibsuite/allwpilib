// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/angle.h>
#include <units/angular_velocity.h>

namespace frc {

/**
 * @ingroup romi_api
 * @{
 */

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 *
 * This class is for the Romi onboard gyro, and will only work in
 * simulation/Romi mode. Only one instance of a RomiGyro is supported.
 */
class RomiGyro {
 public:
  RomiGyro();

  /**
   * Return the actual angle in radians that the robot is currently facing.
   *
   * The angle is based on integration of the returned rate form the gyro.
   * The angle is continuous, that is, it will continue from 2π->3π radians.
   * This allows algorithms that wouldn't want to see a discontinuity in the
   * gyro output as it sweeps from 2π to 0 on the second time around.
   *
   * @return The current heading of the robot.
   */
  units::radian_t GetAngle() const;

  /**
   * Return the rate of rotation of the gyro
   *
   * The rate is based on the most recent reading of the gyro.
   *
   * @return The current rate.
   */
  units::radians_per_second_t GetRate() const;

  /**
   * Get the rate of turn in around the X-axis.
   *
   * @return Rate of turn.
   */
  units::radians_per_second_t GetRateX() const;

  /**
   * Get the rate of turn in around the Y-axis.
   *
   * @return Rate of turn.
   */
  units::radians_per_second_t GetRateY() const;

  /**
   * Get the rate of turn around the Z-axis.
   *
   * @return Rate of turn.
   */
  units::radians_per_second_t GetRateZ() const;

  /**
   * Get the currently reported angle around the X-axis.
   *
   * @return Current angle around X-axis.
   */
  units::radian_t GetAngleX() const;

  /**
   * Get the currently reported angle around the Y-axis.
   *
   * @return Current angle around Y-axis.
   */
  units::radian_t GetAngleY() const;

  /**
   * Get the currently reported angle around the Z-axis.
   *
   * @return Current angle around Z-axis.
   */
  units::radian_t GetAngleZ() const;

  /**
   * Resets the gyro
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

  double m_angleXOffset = 0;
  double m_angleYOffset = 0;
  double m_angleZOffset = 0;
};

/** @} */

}  // namespace frc
