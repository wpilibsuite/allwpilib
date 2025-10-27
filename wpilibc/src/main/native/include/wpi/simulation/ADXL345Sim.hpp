// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/hal/SimDevice.h>

namespace wpi {

class ADXL345_I2C;

namespace sim {

/**
 * Class to control a simulated ADXL345.
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
  wpi::hal::SimDouble m_simX;
  wpi::hal::SimDouble m_simY;
  wpi::hal::SimDouble m_simZ;
};

}  // namespace sim
}  // namespace wpi
