// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/length.h>

namespace frc {

class Ultrasonic;

namespace sim {

/**
 * Class to control a simulated ADXRS450 gyroscope.
 */
class UltrasonicSim {
 public:
  /**
   * Constructs from a ADXRS450_Gyro object.
   *
   * @param gyro ADXRS450_Gyro to simulate
   */
  explicit UltrasonicSim(const Ultrasonic& gyro);

  /**
   * Sets if the range measurement is valid.
   *
   * @param isValid True if valid
   */
  void SetRangeValid(bool isValid);

  /**
   * Sets the range measurement
   *
   * @param range The range
   */
  void SetRange(units::meter_t range);

 private:
  hal::SimBoolean m_simRangeValid;
  hal::SimDouble m_simRange;
};

}  // namespace sim
}  // namespace frc
