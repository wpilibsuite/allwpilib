// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/angle.h>

#include "frc/geometry/Rotation2d.h"

namespace frc {

class AnalogEncoder;

namespace sim {

/**
 * Class to control a simulated analog encoder.
 */
class AnalogEncoderSim {
 public:
  /**
   * Constructs from an AnalogEncoder object.
   *
   * @param encoder AnalogEncoder to simulate
   */
  explicit AnalogEncoderSim(const AnalogEncoder& encoder);

  /**
   * Set the position.
   *
   * @param value The position.
   */
  void Set(double value);

  /**
   * Get the simulated position.
   */
  double Get();

 private:
  hal::SimDouble m_positionSim;
};
}  // namespace sim
}  // namespace frc
