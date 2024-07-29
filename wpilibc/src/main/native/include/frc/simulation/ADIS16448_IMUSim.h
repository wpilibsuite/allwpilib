// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_velocity.h>

namespace frc {

class ADIS16448_IMU;

namespace sim {

/**
 * Class to control a simulated ADIS16448 IMU.
 */
class ADIS16448_IMUSim {
 public:
  /**
   * Constructs from a ADIS16448_IMU object.
   *
   * @param imu ADIS16448_IMU to simulate
   */
  explicit ADIS16448_IMUSim(const ADIS16448_IMU& imu);

  /**
   * Sets the X axis angle (CCW positive).
   *
   * @param angle The angle.
   */
  void SetGyroAngleX(units::degree_t angle);

  /**
   * Sets the Y axis angle (CCW positive).
   *
   * @param angle The angle.
   */
  void SetGyroAngleY(units::degree_t angle);

  /**
   * Sets the Z axis angle (CCW positive).
   *
   * @param angle The angle.
   */
  void SetGyroAngleZ(units::degree_t angle);

  /**
   * Sets the X axis angular rate (CCW positive).
   *
   * @param angularRate The angular rate.
   */
  void SetGyroRateX(units::degrees_per_second_t angularRate);

  /**
   * Sets the Y axis angular rate (CCW positive).
   *
   * @param angularRate The angular rate.
   */
  void SetGyroRateY(units::degrees_per_second_t angularRate);

  /**
   * Sets the Z axis angular rate (CCW positive).
   *
   * @param angularRate The angular rate.
   */
  void SetGyroRateZ(units::degrees_per_second_t angularRate);

  /**
   * Sets the X axis acceleration.
   *
   * @param accel The acceleration.
   */
  void SetAccelX(units::meters_per_second_squared_t accel);

  /**
   * Sets the Y axis acceleration.
   *
   * @param accel The acceleration.
   */
  void SetAccelY(units::meters_per_second_squared_t accel);

  /**
   * Sets the Z axis acceleration.
   *
   * @param accel The acceleration.
   */
  void SetAccelZ(units::meters_per_second_squared_t accel);

 private:
  hal::SimDouble m_simGyroAngleX;
  hal::SimDouble m_simGyroAngleY;
  hal::SimDouble m_simGyroAngleZ;
  hal::SimDouble m_simGyroRateX;
  hal::SimDouble m_simGyroRateY;
  hal::SimDouble m_simGyroRateZ;
  hal::SimDouble m_simAccelX;
  hal::SimDouble m_simAccelY;
  hal::SimDouble m_simAccelZ;
};

}  // namespace sim
}  // namespace frc
