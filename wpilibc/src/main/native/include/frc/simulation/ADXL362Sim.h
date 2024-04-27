// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>

namespace frc {

class ADXL362;

namespace sim {

/**
 * Class to control a simulated ADXL362.
 */
class ADXL362Sim {
 public:
  /**
   * Constructs from a ADXL362 object.
   *
   * @param accel ADXL362 accel to simulate
   */
  explicit ADXL362Sim(const ADXL362& accel);

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
