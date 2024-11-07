// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/geometry/Rotation2d.h>

#include <hal/SimDevice.h>

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
   * Return the actual angle in degrees that the robot is currently facing.
   *
   * The angle is based on integration of the returned rate form the gyro.
   * The angle is continuous, that is, it will continue from 360->361 degrees.
   * This allows algorithms that wouldn't want to see a discontinuity in the
   * gyro output as it sweeps from 360 to 0 on the second time around.
   *
   * @return the current heading of the robot in degrees.
   */
  double GetAngle() const;

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
   * @return the current rate in degrees per second
   */
  double GetRate() const;

  /**
   * Gets the rate of turn in degrees-per-second around the X-axis.
   *
   * @return rate of turn in degrees-per-second
   */
  double GetRateX() const;

  /**
   * Gets the rate of turn in degrees-per-second around the Y-axis.
   *
   * @return rate of turn in degrees-per-second
   */
  double GetRateY() const;

  /**
   * Gets the rate of turn in degrees-per-second around the Z-axis.
   *
   * @return rate of turn in degrees-per-second
   */
  double GetRateZ() const;

  /**
   * Gets the currently reported angle around the X-axis.
   *
   * @return current angle around X-axis in degrees
   */
  double GetAngleX() const;

  /**
   * Gets the currently reported angle around the Y-axis.
   *
   * @return current angle around Y-axis in degrees
   */
  double GetAngleY() const;

  /**
   * Gets the currently reported angle around the Z-axis.
   *
   * @return current angle around Z-axis in degrees
   */
  double GetAngleZ() const;

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

  double m_angleXOffset = 0;
  double m_angleYOffset = 0;
  double m_angleZOffset = 0;
};

/** @} */

}  // namespace frc
