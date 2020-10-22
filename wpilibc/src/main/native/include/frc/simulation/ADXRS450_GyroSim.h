/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/SimDevice.h>
#include <units/angle.h>
#include <units/angular_velocity.h>

#include "frc/geometry/Rotation2d.h"

namespace frc {

class ADXRS450_Gyro;

namespace sim {

/**
 * Class to control a simulated ADXRS450 gyroscope.
 */
class ADXRS450_GyroSim {
 public:
  /**
   * Constructs from a ADXRS450_Gyro object.
   *
   * @param gyro ADXRS450_Gyro to simulate
   */
  explicit ADXRS450_GyroSim(const ADXRS450_Gyro& gyro);

  /**
   * Sets the angle.
   *
   * @param angle The angle (clockwise positive).
   */
  void SetAngle(units::degree_t angle);

  /**
   * Sets the angular rate (clockwise positive).
   *
   * @param rate The angular rate.
   */
  void SetRate(units::degrees_per_second_t rate);

 private:
  hal::SimDouble m_simAngle;
  hal::SimDouble m_simRate;
};

}  // namespace sim
}  // namespace frc
