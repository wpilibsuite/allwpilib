// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/angle.h>

namespace frc {

class DutyCycleEncoder;

namespace sim {

/**
 * Class to control a simulated duty cycle encoder.
 */
class DutyCycleEncoderSim {
 public:
  /**
   * Constructs from a DutyCycleEncoder object.
   *
   * @param dutyCycleEncoder DutyCycleEncoder to simulate
   */
  explicit DutyCycleEncoderSim(const DutyCycleEncoder& encoder);

  /**
   * Set the position tin turns.
   *
   * @param turns The position.
   */
  void Set(units::turn_t turns);

  /**
   * Set the position.
   */
  void SetDistance(double distance);

 private:
  hal::SimDouble m_simPosition;
  hal::SimDouble m_simDistancePerRotation;
};

}  // namespace sim
}  // namespace frc
