// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>

namespace frc {

class ADXL345_SPI;
class ADXL345_I2C;

namespace sim {

/**
 * Class to control a simulated ADXRS450 gyroscope.
 */
class ADXL345Sim {
 public:
  /**
   * Constructs from a ADXL345_I2C object.
   *
   * @param accel ADXL345 accel to simulate
   */
  explicit ADXL345Sim(const ADXL345_I2C& accel);

  /**
   * Constructs from a ADXL345_SPI object.
   *
   * @param accel ADXL345 accel to simulate
   */
  explicit ADXL345Sim(const ADXL345_SPI& accel);

  /**
   * Sets the X acceleration.
   *
   * @param accel The X acceleration.
   */
  void SetX(double accel);

  /**
   * Sets the Y acceleration.
   *
   * @param accel The Y acceleration.
   */
  void SetY(double accel);

  /**
   * Sets the Z acceleration.
   *
   * @param accel The Z acceleration.
   */
  void SetZ(double accel);

 private:
  hal::SimDouble m_simX;
  hal::SimDouble m_simY;
  hal::SimDouble m_simZ;
};

}  // namespace sim
}  // namespace frc
