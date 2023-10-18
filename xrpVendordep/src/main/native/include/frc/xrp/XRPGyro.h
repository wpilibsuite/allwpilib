// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>

namespace frc {

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 *
 * This class is for the XRP onboard gyro, and will only work in
 * simulation/XRP mode. Only one instance of a XRPGyro is supported.
 */
class XRPGyro {
 public:
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
   * Return the rate of rotation of the gyro
   *
   * The rate is based on the most recent reading of the gyro.
   *
   * @return the current rate in degrees per second
   */
  double GetRate() const;

  /**
   * Gets the rate of turn in degrees-per-second around the X-axis
   */
  double GetRateX() const;

  /**
   * Gets the rate of turn in degrees-per-second around the Y-axis
   */
  double GetRateY() const;

  /**
   * Gets the rate of turn in degrees-per-second around the Z-axis
   */
  double GetRateZ() const;

  /**
   * Gets the currently reported angle around the X-axis
   */
  double GetAngleX() const;

  /**
   * Gets the currently reported angle around the X-axis
   */
  double GetAngleY() const;

  /**
   * Gets the currently reported angle around the X-axis
   */
  double GetAngleZ() const;

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

}  // namespace frc
