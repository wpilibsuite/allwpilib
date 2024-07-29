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
 * Class to control a simulated {@link Ultrasonic}.
 */
class UltrasonicSim {
 public:
  /**
   * Constructor.
   *
   * @param ultrasonic The real ultrasonic to simulate
   */
  explicit UltrasonicSim(const Ultrasonic& ultrasonic);

  /**
   * Constructor.
   *
   * @param ping unused.
   * @param echo the ultrasonic's echo channel.
   */
  UltrasonicSim(int ping, int echo);

  /**
   * Sets if the range measurement is valid.
   *
   * @param valid True if valid
   */
  void SetRangeValid(bool valid);

  /**
   * Sets the range measurement.
   *
   * @param range The range.
   */
  void SetRange(units::inch_t range);

 private:
  hal::SimBoolean m_simRangeValid;
  hal::SimDouble m_simRange;
};

}  // namespace sim
}  // namespace frc
